#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <sstream>

// Cross-platform socket definitions (Linux focus)
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

// Constants
#define TCP_PORT 9000
#define UDP_PORT 9001
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 4096

// Helper function to get current timestamp as a string
inline std::string currentTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    char buf[100];
    ctime_r(&now_c, buf);  // Use ctime_r for thread-safe Linux version
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

#endif // COMMON_UTILS_H
