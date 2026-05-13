#include "client.h"
#include <iostream>
#include <signal.h>

CampusClient* globalClient = nullptr;

void signalHandler(int signum) {
    std::cout << "\n[CLIENT] Shutting down...\n";
    exit(0);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: ./client <CampusName>\n";
        std::cout << "Example: ./client Lahore\n";
        std::cout << "\nAvailable campuses:\n";
        std::cout << "  - Lahore\n";
        std::cout << "  - Karachi\n";
        std::cout << "  - Peshawar\n";
        std::cout << "  - Islamabad\n";
        std::cout << "  - Multan\n";
        return 1;
    }

    // Setup signal handlers for graceful shutdown
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    std::string campusName = argv[1];

    std::cout << "=== NU-Information Exchange System - Campus Client ===\n";
    std::cout << "Campus: " << campusName << "\n";
    std::cout << "Connecting to Central Server...\n";

    CampusClient client(campusName);
    globalClient = &client;

    if (!client.connectToServer()) {
        std::cerr << "[ERROR] Failed to connect to server\n";
        return 1;
    }

    client.authenticate();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    client.start();

    return 0;
}
