#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <chrono>
#include <ctime>

// Port definitions
#define TCP_PORT 9000
#define UDP_PORT 9001
#define SERVER_IP "127.0.0.1"

// Buffer size
#define BUFFER_SIZE 4096

// Message protocol formats
#define AUTH_MSG "AUTH"
#define MSG_TYPE "MSG"
#define HEARTBEAT_MSG "HEARTBEAT"
#define BROADCAST_MSG "BROADCAST"

// Helper function to get current timestamp as a string
inline std::string currentTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    char buf[100];
    ctime_r(&now_c, buf);
    std::string timeStr(buf);
    timeStr.erase(timeStr.find_last_not_of(" \n\r\t") + 1);
    return timeStr;
}

// Helper function to split a string by a delimiter
inline std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

#endif // COMMON_H
