#include "server.h"
#include "../../include/common.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

CentralServer::CentralServer() : tcpSocket(-1), udpSocket(-1) {}

CentralServer::~CentralServer() {
    if (tcpSocket != -1) close(tcpSocket);
    if (udpSocket != -1) close(udpSocket);
}

bool CentralServer::initialize() {
    // Create TCP socket
    tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (tcpSocket < 0) {
        std::cerr << "Failed to create TCP socket\n";
        return false;
    }

    // Enable address reuse
    int opt = 1;
    if (setsockopt(tcpSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt)) < 0) {
        std::cerr << "Failed to set socket option\n";
        return false;
    }

    // Bind TCP socket
    struct sockaddr_in tcpAddr;
    tcpAddr.sin_family = AF_INET;
    tcpAddr.sin_port = htons(TCP_PORT);
    tcpAddr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(tcpSocket, (struct sockaddr*)&tcpAddr, sizeof(tcpAddr)) < 0) {
        std::cerr << "Failed to bind TCP socket to port " << TCP_PORT << "\n";
        return false;
    }

    // Listen on TCP socket
    if (listen(tcpSocket, 5) < 0) {
        std::cerr << "Failed to listen on TCP socket\n";
        return false;
    }

    std::cout << "[SERVER] TCP Server listening on port " << TCP_PORT << "\n";

    // Create UDP socket
    udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSocket < 0) {
        std::cerr << "Failed to create UDP socket\n";
        return false;
    }

    // Bind UDP socket
    struct sockaddr_in udpAddr;
    udpAddr.sin_family = AF_INET;
    udpAddr.sin_port = htons(UDP_PORT);
    udpAddr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(udpSocket, (struct sockaddr*)&udpAddr, sizeof(udpAddr)) < 0) {
        std::cerr << "Failed to bind UDP socket to port " << UDP_PORT << "\n";
        return false;
    }

    std::cout << "[SERVER] UDP Server listening on port " << UDP_PORT << "\n";
    return true;
}

void CentralServer::handleClient(int clientSock) {
    char buffer[BUFFER_SIZE];
    std::string campusName = "";

    // 1. Authenticate the client
    int bytesRead = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        std::string authMsg(buffer);
        auto tokens = split(authMsg, '|');
        
        if (tokens.size() >= 2 && tokens[0] == AUTH_MSG) {
            auto creds = split(tokens[1], ',');
            if (creds.size() == 2) {
                std::string campStr = creds[0];
                std::string passStr = creds[1];
                
                if (campStr.find("Campus:") == 0 && passStr.find("Pass:") == 0) {
                    campusName = campStr.substr(7);
                    std::string pass = passStr.substr(5);
                    
                    // Simple password validation
                    if (pass == "123" || pass == "NU-" + campusName + "-123") {
                        std::cout << "[AUTH] " << campusName << " authenticated successfully.\n";
                        {
                            std::lock_guard<std::mutex> lock(mtx);
                            tcp_clients[campusName] = clientSock;
                        }
                    } else {
                        std::cout << "[AUTH] " << campusName << " failed authentication.\n";
                        close(clientSock);
                        return;
                    }
                }
            }
        }
    } else {
        close(clientSock);
        return;
    }

    if (campusName.empty()) {
        close(clientSock);
        return;
    }

    // 2. Continually receive and route messages from this client
    while (true) {
        bytesRead = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::string msg(buffer);
            auto tokens = split(msg, '|');
            
            // Format: MSG|<SrcCampus>|<SrcDept>|<DstCampus>|<DstDept>|<MessageText>
            if (tokens.size() >= 6 && tokens[0] == MSG_TYPE) {
                std::string srcCamp = tokens[1];
                std::string srcDept = tokens[2];
                std::string dstCamp = tokens[3];
                std::string dstDept = tokens[4];
                std::string msgText = tokens[5];
                for (size_t i = 6; i < tokens.size(); ++i) msgText += "|" + tokens[i];

                std::cout << "[ROUTING] Message from " << srcCamp << " (" << srcDept 
                          << ") to " << dstCamp << " (" << dstDept << ")\n";

                {
                    std::lock_guard<std::mutex> lock(mtx);
                    if (tcp_clients.find(dstCamp) != tcp_clients.end()) {
                        int dstSock = tcp_clients[dstCamp];
                        send(dstSock, msg.c_str(), msg.size(), 0);
                    } else {
                        std::cout << "[ROUTING ERROR] Destination campus " << dstCamp << " not connected.\n";
                    }
                }
            }
        } else {
            // Client disconnected
            std::cout << "[DISCONNECT] " << campusName << " disconnected.\n";
            {
                std::lock_guard<std::mutex> lock(mtx);
                tcp_clients.erase(campusName);
                last_heartbeat.erase(campusName);
            }
            close(clientSock);
            break;
        }
    }
}

void CentralServer::udpListenerThread() {
    char buffer[BUFFER_SIZE];
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    std::cout << "[UDP] Heartbeat listener thread started\n";

    while (true) {
        int bytesRead = recvfrom(udpSocket, buffer, sizeof(buffer) - 1, 0, 
                                  (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::string msg(buffer);
            auto tokens = split(msg, '|');
            
            // Format: HEARTBEAT|<CampusName>|<Timestamp>
            if (tokens.size() >= 3 && tokens[0] == HEARTBEAT_MSG) {
                std::string campusName = tokens[1];
                std::string timestamp = tokens[2];
                for (size_t i = 3; i < tokens.size(); ++i) timestamp += "|" + tokens[i];

                {
                    std::lock_guard<std::mutex> lock(mtx);
                    last_heartbeat[campusName] = timestamp;
                    std::cout << "[HEARTBEAT] " << campusName << " is online\n";
                }
            }
        }
    }
}

void CentralServer::broadcastMessage(const std::string& message) {
    std::lock_guard<std::mutex> lock(mtx);
    
    for (const auto& campus : tcp_clients) {
        std::string broadcastMsg = BROADCAST_MSG + std::string("|") + message;
        send(campus.second, broadcastMsg.c_str(), broadcastMsg.size(), 0);
    }
    std::cout << "[BROADCAST] Message sent to all campuses\n";
}

void CentralServer::adminConsoleThread() {
    std::cout << "[ADMIN] Admin console started\n";
    
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "\n=== Admin Menu ===\n";
        std::cout << "1. List Connected Campuses\n";
        std::cout << "2. Broadcast Announcement\n";
        std::cout << "3. Exit\n";
        std::cout << "Select option: ";
        
        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "1") {
            {
                std::lock_guard<std::mutex> lock(mtx);
                std::cout << "--- Connected Campuses ---\n";
                if (tcp_clients.empty()) {
                    std::cout << "No campuses currently connected.\n";
                } else {
                    for (const auto& campus : tcp_clients) {
                        std::cout << "Campus: " << campus.first << " | TCP: Connected | Last Heartbeat: " 
                                  << last_heartbeat[campus.first] << "\n";
                    }
                }
            }
        } else if (choice == "2") {
            std::cout << "Enter broadcast message: ";
            std::string msg;
            std::getline(std::cin, msg);
            broadcastMessage(msg);
        } else if (choice == "3") {
            std::cout << "[ADMIN] Exiting admin console\n";
            break;
        }
    }
}

void CentralServer::start() {
    // Start UDP listener thread
    std::thread udpThread(&CentralServer::udpListenerThread, this);
    udpThread.detach();

    // Start admin console thread
    std::thread adminThread(&CentralServer::adminConsoleThread, this);
    adminThread.detach();

    std::cout << "[SERVER] Central Server started and ready to accept connections\n";

    // Accept incoming TCP connections
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    while (true) {
        int clientSock = accept(tcpSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSock < 0) {
            std::cerr << "Accept failed\n";
            continue;
        }

        std::cout << "[CONNECTION] New client connected\n";

        // Handle client in a separate thread
        std::thread clientThread(&CentralServer::handleClient, this, clientSock);
        clientThread.detach();
    }
}
