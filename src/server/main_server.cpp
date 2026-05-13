#include "server.h"
#include <iostream>
#include <signal.h>

CentralServer* globalServer = nullptr;

void signalHandler(int signum) {
    std::cout << "\n[SERVER] Shutting down...\n";
    exit(0);
}

int main() {
    // Setup signal handlers for graceful shutdown
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    std::cout << "=== NU-Information Exchange System - Central Server ===\n";
    std::cout << "Initializing server...\n";

    CentralServer server;
    globalServer = &server;

    if (!server.initialize()) {
        std::cerr << "Failed to initialize server\n";
        return 1;
    }

    std::cout << "[SERVER] Server initialized successfully\n";
    std::cout << "[SERVER] Starting server...\n";

    server.start();

    return 0;
}
