// client.cpp
// Campus Client for NU-Information Exchange System

#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>
#include <sstream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1"
#define TCP_PORT 9000
#define UDP_PORT 9001

std::vector<std::string> messages;
std::mutex mtx;
bool running = true;

// Helper function to get current timestamp
std::string currentTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    char buf[100];
    ctime_s(buf, sizeof(buf), &now_c);
    std::string timeStr(buf);
    timeStr.erase(timeStr.find_last_not_of(" \n\r\t") + 1);
    return timeStr;
}

// Thread function to receive incoming TCP messages
void tcpReceiveThread(SOCKET tcpSock) {
    char buffer[4096];
    while (running) {
        int bytesRead = recv(tcpSock, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::lock_guard<std::mutex> lock(mtx);
            messages.push_back("[TCP MESSAGE] " + std::string(buffer));
        } else {
            std::cout << "\n[DISCONNECTED] Server connection lost.\n";
            running = false;
            break;
        }
    }
}

// Thread function to receive incoming UDP broadcasts
void udpReceiveThread(SOCKET udpSock) {
    char buffer[4096];
    sockaddr_in senderAddr;
    int senderAddrLen = sizeof(senderAddr);
    while (running) {
        int bytesRead = recvfrom(udpSock, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&senderAddr, &senderAddrLen);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::lock_guard<std::mutex> lock(mtx);
            messages.push_back("[UDP BROADCAST] " + std::string(buffer));
        }
    }
}

// Thread function to send periodic UDP heartbeats
void heartbeatThread(SOCKET udpSock, const std::string& campusName) {
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(UDP_PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    while (running) {
        std::string pkt = "HEARTBEAT|" + campusName + "|" + currentTime();
        sendto(udpSock, pkt.c_str(), pkt.size(), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
        
        // Wait for 10 seconds before sending the next heartbeat
        for (int i = 0; i < 100 && running; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: client.exe <CampusName>\n";
        std::cout << "Example: client.exe Lahore\n";
        return 1;
    }
    std::string campusName = argv[1];

    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return 1;
    }

    // 1. Establish TCP Connection to Server
    SOCKET tcpSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(TCP_PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    if (connect(tcpSock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "Failed to connect to Central Server at " << SERVER_IP << ":" << TCP_PORT << "\n";
        WSACleanup();
        return 1;
    }

    // 2. Perform Authentication
    std::string pass = "123"; // Dummy password for flexibility
    std::string authMsg = "AUTH|Campus:" + campusName + ",Pass:" + pass;
    send(tcpSock, authMsg.c_str(), authMsg.size(), 0);

    // 3. Setup UDP Socket (used for both sending heartbeats and receiving broadcasts)
    SOCKET udpSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    sockaddr_in localAddr;
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(0); // Bind to any available port
    localAddr.sin_addr.s_addr = INADDR_ANY;
    bind(udpSock, (sockaddr*)&localAddr, sizeof(localAddr));

    // Launch background threads
    std::thread t1(tcpReceiveThread, tcpSock);
    t1.detach();

    std::thread t2(udpReceiveThread, udpSock);
    t2.detach();

    std::thread t3(heartbeatThread, udpSock, campusName);
    t3.detach();

    // 4. Console Interface for Department Users
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "\n=== NU Campus Client - " << campusName << " ===\n";
        std::cout << "1. Send a message to another campus\n";
        std::cout << "2. View received messages\n";
        std::cout << "3. Exit\n";
        std::cout << "Enter choice: ";
        
        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "1") {
            std::cout << "Enter target campus : ";
            std::string targetCampus;
            std::getline(std::cin, targetCampus);

            std::cout << "Enter target dept : ";
            std::string targetDept;
            std::getline(std::cin, targetDept);

            std::cout << "Enter your dept : ";
            std::string myDept;
            std::getline(std::cin, myDept);

            std::cout << "Enter your message : ";
            std::string msgText;
            std::getline(std::cin, msgText);

            // Format: MSG|<SrcCampus>|<SrcDept>|<DstCampus>|<DstDept>|<MessageText>
            std::string msg = "MSG|" + campusName + "|" + myDept + "|" + targetCampus + "|" + targetDept + "|" + msgText;
            send(tcpSock, msg.c_str(), msg.size(), 0);
            std::cout << "[SENT] " << msg << "\n";

        } else if (choice == "2") {
            std::lock_guard<std::mutex> lock(mtx);
            std::cout << "\n--- Received Messages ---\n";
            if (messages.empty()) {
                std::cout << "No new messages.\n";
            } else {
                for (const auto& m : messages) {
                    std::cout << m << "\n";
                }
                messages.clear(); // Clear messages after viewing
            }
        } else if (choice == "3") {
            running = false;
            break;
        }
    }

    // Cleanup
    closesocket(tcpSock);
    closesocket(udpSock);
    WSACleanup();

    return 0;
}
