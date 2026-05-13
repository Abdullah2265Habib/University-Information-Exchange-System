// server.cpp
// Central Server for NU-Information Exchange System

#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <map>
#include <mutex>
#include <chrono>
#include <sstream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define TCP_PORT 9000
#define UDP_PORT 9001

// Global data structures to maintain connected clients and their state
std::map<std::string, SOCKET> tcp_clients;
std::map<std::string, sockaddr_in> udp_clients;
std::map<std::string, std::string> last_heartbeat;
std::mutex mtx; // Mutex for thread-safe access to global maps

// Helper function to get current timestamp as a string
std::string currentTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    char buf[100];
    ctime_s(buf, sizeof(buf), &now_c);
    std::string timeStr(buf);
    timeStr.erase(timeStr.find_last_not_of(" \n\r\t") + 1);
    return timeStr;
}

// Helper function to split a string by a delimiter
std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Thread function to handle a connected TCP client
void handleClient(SOCKET clientSock) {
    char buffer[4096];
    std::string campusName = "";

    // 1. Authenticate the client
    int bytesRead = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        std::string authMsg(buffer);
        // Expect format: AUTH|Campus:<CampusName>,Pass:<Password>
        auto tokens = split(authMsg, '|');
        if (tokens.size() >= 2 && tokens[0] == "AUTH") {
            auto creds = split(tokens[1], ',');
            if (creds.size() == 2) {
                std::string campStr = creds[0];
                std::string passStr = creds[1];
                if (campStr.find("Campus:") == 0 && passStr.find("Pass:") == 0) {
                    campusName = campStr.substr(7);
                    std::string pass = passStr.substr(5);
                    
                    // Simple password validation for demonstration
                    if (pass == "123" || pass == "NU-" + campusName + "-123") { 
                        std::cout << "[AUTH] " << campusName << " authenticated successfully.\n";
                        std::lock_guard<std::mutex> lock(mtx);
                        tcp_clients[campusName] = clientSock;
                    } else {
                        std::cout << "[AUTH] " << campusName << " failed authentication.\n";
                        closesocket(clientSock);
                        return;
                    }
                }
            }
        }
    } else {
        closesocket(clientSock);
        return;
    }

    if (campusName.empty()) {
        closesocket(clientSock);
        return;
    }

    // 2. Continually receive and route messages from this client
    while (true) {
        bytesRead = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::string msg(buffer);
            auto tokens = split(msg, '|');
            // Expect format: MSG|<SrcCampus>|<SrcDept>|<DstCampus>|<DstDept>|<MessageText>
            if (tokens.size() >= 6 && tokens[0] == "MSG") {
                std::string srcCamp = tokens[1];
                std::string srcDept = tokens[2];
                std::string dstCamp = tokens[3];
                std::string dstDept = tokens[4];
                // Handle message text that may contain pipes
                std::string msgText = tokens[5];
                for (size_t i = 6; i < tokens.size(); ++i) msgText += "|" + tokens[i];

                std::cout << "[ROUTING] Message from " << srcCamp << " (" << srcDept << ") to " << dstCamp << " (" << dstDept << ")\n";

                std::lock_guard<std::mutex> lock(mtx);
                if (tcp_clients.find(dstCamp) != tcp_clients.end()) {
                    SOCKET dstSock = tcp_clients[dstCamp];
                    send(dstSock, msg.c_str(), msg.size(), 0);
                } else {
                    std::cout << "[ROUTING ERROR] Destination campus " << dstCamp << " not connected.\n";
                }
            }
        } else {
            // Client disconnected or error occurred
            std::cout << "[DISCONNECT] " << campusName << " disconnected.\n";
            std::lock_guard<std::mutex> lock(mtx);
            tcp_clients.erase(campusName);
            udp_clients.erase(campusName);
            last_heartbeat.erase(campusName);
            closesocket(clientSock);
            break;
        }
    }
}

// Thread function to receive UDP heartbeats from clients
void udpThread() {
    SOCKET udpSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(UDP_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(udpSock, (sockaddr*)&serverAddr, sizeof(serverAddr));

    char buffer[4096];
    sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);

    while (true) {
        int bytesRead = recvfrom(udpSock, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&clientAddr, &clientAddrLen);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::string msg(buffer);
            auto tokens = split(msg, '|');
            // Expect format: HEARTBEAT|<CampusName>|<Timestamp>
            if (tokens.size() >= 3 && tokens[0] == "HEARTBEAT") {
                std::string campusName = tokens[1];
                // Combine remaining tokens in case timestamp has pipes
                std::string timestamp = tokens[2];
                for (size_t i = 3; i < tokens.size(); ++i) timestamp += "|" + tokens[i];

                std::lock_guard<std::mutex> lock(mtx);
                last_heartbeat[campusName] = timestamp;
                udp_clients[campusName] = clientAddr; // Save client UDP address for broadcasts
            }
        }
    }
}

// Admin thread for console interaction
void adminThread() {
    SOCKET broadcastSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    // Enable broadcast on this socket just in case, though we are doing direct sends to known UDP endpoints
    int optval = 1;
    setsockopt(broadcastSock, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(optval));

    while (true) {
        // Sleep slightly to prevent menu from garbling output immediately
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "\n=== Admin Menu ===\n";
        std::cout << "1. List Connected Campuses\n";
        std::cout << "2. Broadcast Announcement\n";
        std::cout << "Select option: ";
        
        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "1") {
            std::lock_guard<std::mutex> lock(mtx);
            std::cout << "--- Connected Campuses ---\n";
            if (tcp_clients.empty()) {
                std::cout << "No campuses currently connected.\n";
            }
            for (const auto& pair : tcp_clients) {
                std::cout << "Campus: " << pair.first << " | TCP: Connected | Last Heartbeat: " << last_heartbeat[pair.first] << "\n";
            }
        } else if (choice == "2") {
            std::cout << "Enter broadcast message: ";
            std::string msg;
            std::getline(std::cin, msg);
            std::string broadcastMsg = "BROADCAST|" + msg;

            std::lock_guard<std::mutex> lock(mtx);
            for (const auto& pair : udp_clients) {
                // Send broadcast message individually to each known UDP endpoint
                sendto(broadcastSock, broadcastMsg.c_str(), broadcastMsg.size(), 0, (sockaddr*)&pair.second, sizeof(pair.second));
            }
            std::cout << "[BROADCAST SENT]\n";
        }
    }
}

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return 1;
    }

    // Create main TCP listening socket
    SOCKET tcpSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(TCP_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(tcpSock, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(tcpSock, SOMAXCONN);

    std::cout << "Central Server started. Listening on TCP " << TCP_PORT << " and UDP " << UDP_PORT << "\n";

    // Launch background threads
    std::thread udpT(udpThread);
    udpT.detach();

    std::thread adminT(adminThread);
    adminT.detach();

    // Accept loop for incoming TCP connections
    while (true) {
        sockaddr_in clientAddr;
        int clientAddrLen = sizeof(clientAddr);
        SOCKET clientSock = accept(tcpSock, (sockaddr*)&clientAddr, &clientAddrLen);
        
        if (clientSock != INVALID_SOCKET) {
            std::thread t(handleClient, clientSock);
            t.detach();
        }
    }

    // Cleanup
    closesocket(tcpSock);
    WSACleanup();
    return 0;
}
