#include "client.h"
#include "../../include/common.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <thread>

CampusClient::CampusClient(const std::string& name) 
    : campusName(name), tcpSocket(-1), udpSocket(-1), running(true) {}

CampusClient::~CampusClient() {
    if (tcpSocket != -1) close(tcpSocket);
    if (udpSocket != -1) close(udpSocket);
    running = false;
}

bool CampusClient::connectToServer() {
    // Create and connect TCP socket
    tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (tcpSocket < 0) {
        std::cerr << "Failed to create TCP socket\n";
        return false;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(TCP_PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    if (connect(tcpSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Failed to connect to Central Server at " << SERVER_IP << ":" << TCP_PORT << "\n";
        return false;
    }

    std::cout << "[CLIENT] Connected to Central Server\n";

    // Create UDP socket for heartbeats and broadcasts
    udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSocket < 0) {
        std::cerr << "Failed to create UDP socket\n";
        return false;
    }

    struct sockaddr_in localAddr;
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(0);  // Bind to any available port
    localAddr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(udpSocket, (struct sockaddr*)&localAddr, sizeof(localAddr)) < 0) {
        std::cerr << "Failed to bind UDP socket\n";
        return false;
    }

    std::cout << "[CLIENT] UDP socket created\n";
    return true;
}

void CampusClient::authenticate() {
    std::string pass = "123";  // Default password
    std::string authMsg = "AUTH|Campus:" + campusName + ",Pass:" + pass;
    
    if (send(tcpSocket, authMsg.c_str(), authMsg.size(), 0) < 0) {
        std::cerr << "Failed to send authentication message\n";
        running = false;
        return;
    }
    
    std::cout << "[CLIENT] Authentication message sent\n";
}

void CampusClient::tcpReceiveThread() {
    char buffer[BUFFER_SIZE];
    
    while (running) {
        int bytesRead = recv(tcpSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            {
                std::lock_guard<std::mutex> lock(mtx);
                messages.push_back("[TCP MESSAGE] " + std::string(buffer));
            }
        } else if (bytesRead == 0) {
            std::cout << "\n[DISCONNECTED] Server connection closed.\n";
            running = false;
            break;
        } else {
            std::cerr << "\n[ERROR] TCP receive error\n";
            running = false;
            break;
        }
    }
}

void CampusClient::udpReceiveThread() {
    char buffer[BUFFER_SIZE];
    struct sockaddr_in senderAddr;
    socklen_t senderAddrLen = sizeof(senderAddr);

    while (running) {
        int bytesRead = recvfrom(udpSocket, buffer, sizeof(buffer) - 1, 0,
                                 (struct sockaddr*)&senderAddr, &senderAddrLen);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            {
                std::lock_guard<std::mutex> lock(mtx);
                messages.push_back("[UDP BROADCAST] " + std::string(buffer));
            }
        }
    }
}

void CampusClient::heartbeatThread() {
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(UDP_PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    std::cout << "[CLIENT] Heartbeat thread started\n";

    while (running) {
        std::string pkt = "HEARTBEAT|" + campusName + "|" + currentTime();
        
        if (sendto(udpSocket, pkt.c_str(), pkt.size(), 0, 
                   (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            std::cerr << "Failed to send heartbeat\n";
        }
        
        // Wait for 10 seconds before sending the next heartbeat
        for (int i = 0; i < 100 && running; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void CampusClient::sendMessage(const std::string& targetCampus, const std::string& targetDept,
                               const std::string& myDept, const std::string& msgText) {
    // Format: MSG|<SrcCampus>|<SrcDept>|<DstCampus>|<DstDept>|<MessageText>
    std::string msg = "MSG|" + campusName + "|" + myDept + "|" + targetCampus + "|" + targetDept + "|" + msgText;
    
    if (send(tcpSocket, msg.c_str(), msg.size(), 0) < 0) {
        std::cerr << "Failed to send message\n";
    } else {
        std::cout << "[SENT] " << msg << "\n";
    }
}

void CampusClient::consoleInterface() {
    std::cout << "\n=== NU Campus Client - " << campusName << " ===\n";
    std::cout << "Connected to Central Server\n\n";

    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "\n=== Menu ===\n";
        std::cout << "1. Send a message to another campus\n";
        std::cout << "2. View received messages\n";
        std::cout << "3. Exit\n";
        std::cout << "Enter choice: ";
        
        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "1") {
            std::cout << "Enter target campus: ";
            std::string targetCampus;
            std::getline(std::cin, targetCampus);

            std::cout << "Enter target department: ";
            std::string targetDept;
            std::getline(std::cin, targetDept);

            std::cout << "Enter your department: ";
            std::string myDept;
            std::getline(std::cin, myDept);

            std::cout << "Enter message: ";
            std::string msgText;
            std::getline(std::cin, msgText);

            sendMessage(targetCampus, targetDept, myDept, msgText);

        } else if (choice == "2") {
            {
                std::lock_guard<std::mutex> lock(mtx);
                std::cout << "\n--- Received Messages ---\n";
                if (messages.empty()) {
                    std::cout << "No new messages.\n";
                } else {
                    for (const auto& m : messages) {
                        std::cout << m << "\n";
                    }
                    messages.clear();  // Clear after viewing
                }
            }
        } else if (choice == "3") {
            running = false;
            std::cout << "[CLIENT] Exiting...\n";
            break;
        } else {
            std::cout << "Invalid choice. Please try again.\n";
        }
    }
}

void CampusClient::start() {
    // Launch TCP receive thread
    std::thread tcpThread(&CampusClient::tcpReceiveThread, this);
    tcpThread.detach();

    // Launch UDP receive thread
    std::thread udpThread(&CampusClient::udpReceiveThread, this);
    udpThread.detach();

    // Launch heartbeat thread
    std::thread hbThread(&CampusClient::heartbeatThread, this);
    hbThread.detach();

    // Run console interface (blocks until user exits)
    consoleInterface();
}
