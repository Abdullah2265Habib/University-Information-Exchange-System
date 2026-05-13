#!/bin/bash
# test.sh - Automated testing script for the application

set -e

echo "=== NU-Information Exchange System - Test Script ==="
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Build the project
echo -e "${YELLOW}[TEST] Building project...${NC}"
make clean
make all

if [ $? -ne 0 ]; then
    echo -e "${RED}[ERROR] Build failed!${NC}"
    exit 1
fi

echo -e "${GREEN}[SUCCESS] Build completed!${NC}"
echo ""

# Function to kill all background processes on exit
cleanup() {
    echo ""
    echo -e "${YELLOW}[TEST] Cleaning up...${NC}"
    pkill -f "bin/server" 2>/dev/null || true
    pkill -f "bin/client" 2>/dev/null || true
    sleep 1
}

trap cleanup EXIT

# Start the server in background
echo -e "${YELLOW}[TEST] Starting Central Server...${NC}"
./bin/server &
SERVER_PID=$!
sleep 2

# Check if server started successfully
if ! kill -0 $SERVER_PID 2>/dev/null; then
    echo -e "${RED}[ERROR] Failed to start server!${NC}"
    exit 1
fi

echo -e "${GREEN}[SUCCESS] Server started (PID: $SERVER_PID)${NC}"
echo ""

# Start multiple clients
CLIENTS=("Lahore" "Karachi" "Peshawar")

for campus in "${CLIENTS[@]}"; do
    echo -e "${YELLOW}[TEST] Starting client: $campus${NC}"
    ./bin/client "$campus" <<EOF &
2
3
EOF
    sleep 1
done

echo -e "${GREEN}[SUCCESS] All clients started!${NC}"
echo ""

# Wait a bit for clients to connect
sleep 3

# Check if all processes are still running
echo -e "${YELLOW}[TEST] Checking process status...${NC}"

if kill -0 $SERVER_PID 2>/dev/null; then
    echo -e "${GREEN}[SUCCESS] Server is still running${NC}"
else
    echo -e "${RED}[ERROR] Server crashed!${NC}"
    exit 1
fi

echo ""
echo -e "${YELLOW}[TEST] Test completed successfully!${NC}"
echo -e "${YELLOW}[TEST] You can manually test by running:${NC}"
echo "  Terminal 1: make run-server"
echo "  Terminal 2: make run-client CAMPUS_NAME=Lahore"
echo "  Terminal 3: make run-client CAMPUS_NAME=Karachi"
