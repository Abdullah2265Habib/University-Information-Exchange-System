#ifndef SERVER_H
#define SERVER_H

#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <memory>

class CentralServer {
private:
    int tcpSocket;
    int udpSocket;
    std::map<std::string, int> tcp_clients;      // Campus name -> TCP socket
    std::map<std::string, std::string> last_heartbeat; // Campus name -> last heartbeat timestamp
    std::mutex mtx;  // Thread-safe access
    
public:
    CentralServer();
    ~CentralServer();
    
    bool initialize();
    void start();
    void handleClient(int clientSock);
    void udpListenerThread();
    void adminConsoleThread();
    void broadcastMessage(const std::string& message);
};

#endif // SERVER_H
