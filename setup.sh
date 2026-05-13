#!/bin/bash
# setup.sh - Build and setup the project

echo "=== NU-Information Exchange System - Setup Script ==="
echo ""

# Check if g++ is installed
if ! command -v g++ &> /dev/null; then
    echo "[ERROR] g++ is not installed. Installing build-essential..."
    sudo apt-get update
    sudo apt-get install -y build-essential
fi

echo "[SETUP] Building the project..."
make clean
make all

if [ $? -eq 0 ]; then
    echo ""
    echo "[SUCCESS] Project built successfully!"
    echo ""
    echo "Available commands:"
    echo "  ./bin/server                  - Start the Central Server"
    echo "  ./bin/client Lahore           - Start Lahore Campus Client"
    echo "  ./bin/client Karachi          - Start Karachi Campus Client"
    echo "  ./bin/client Peshawar         - Start Peshawar Campus Client"
    echo "  ./bin/client Islamabad        - Start Islamabad Campus Client"
    echo "  ./bin/client Multan           - Start Multan Campus Client"
    echo ""
    echo "Or use: make run-server"
    echo "        make run-client CAMPUS_NAME=Lahore"
else
    echo "[ERROR] Build failed!"
    exit 1
fi
