#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <memory>

class CampusClient {
private:
    std::string campusName;
    int tcpSocket;
    int udpSocket;
    std::vector<std::string> messages;
    std::mutex mtx;
    bool running;
    
public:
    CampusClient(const std::string& name);
    ~CampusClient();
    
    bool connectToServer();
    void authenticate();
    void start();
    void tcpReceiveThread();
    void udpReceiveThread();
    void heartbeatThread();
    void consoleInterface();
    void sendMessage(const std::string& targetCampus, const std::string& targetDept, 
                     const std::string& myDept, const std::string& msgText);
};

#endif // CLIENT_H
