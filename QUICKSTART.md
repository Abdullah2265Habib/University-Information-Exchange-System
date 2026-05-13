# Quick Start Guide - Linux / WSL

## One-Time Setup

```bash
# 1. Navigate to project directory
cd "University-Information-Exchange-System"

# 2. Make scripts executable
chmod +x setup.sh test.sh

# 3. Build the project
./setup.sh
```

## Running the Application

### Method 1: Using Make (Recommended)

**Terminal 1 - Start Server:**
```bash
make run-server
```

**Terminal 2 - Start First Client:**
```bash
make run-client CAMPUS_NAME=Lahore
```

**Terminal 3 - Start Second Client:**
```bash
make run-client CAMPUS_NAME=Karachi
```

**Terminal 4 - Start Third Client (Optional):**
```bash
make run-client CAMPUS_NAME=Peshawar
```

### Method 2: Using Direct Execution

**Terminal 1:**
```bash
./bin/server
```

**Terminal 2:**
```bash
./bin/client Lahore
```

**Terminal 3:**
```bash
./bin/client Karachi
```

### Method 4: Using WSL Terminal Split

If your terminal supports split panes (like VS Code integrated terminal):

```bash
# Terminal 1
./bin/server

# Terminal 2 (split pane)
./bin/client Lahore

# Terminal 3 (split pane)
./bin/client Karachi
```

## Available Campus Names

- Lahore
- Karachi
- Peshawar
- Islamabad
- Multan

## Client Menu Options

Once a client is running:
```
=== Menu ===
1. Send a message to another campus
2. View received messages
3. Exit
```

### Sending a Message

Example: Lahore Admissions sends to Karachi Academics
```
Enter choice: 1
Enter target campus: Karachi
Enter target department: Academics
Enter your department: Admissions
Enter message: Please send admission statistics
```

## Server Admin Menu

When the server is running:
```
=== Admin Menu ===
1. List Connected Campuses
2. Broadcast Announcement
```

### Broadcasting a Message

```
Enter choice: 2
Enter broadcast message: System maintenance scheduled for 8 PM
```

All connected clients will receive this broadcast message.

## Troubleshooting

### Port Already in Use
```bash
# Find process using port 9000 or 9001
lsof -i :9000
lsof -i :9001

# Kill the process
kill -9 <PID>
```

### Can't Connect to Server
- Ensure server is running first
- Check that firewall allows ports 9000 and 9001
- Try restarting: `pkill -f "bin/server"`

### Build Issues
```bash
# Clean and rebuild
make clean
make all

# Or use the setup script
./setup.sh
```

## Testing Quickly

```bash
# Automated test (starts server and clients automatically)
chmod +x test.sh
./test.sh
```

## Stopping the Application

**In each terminal:**
```bash
# For clients: Select option 3 from menu or press Ctrl+C
# For server: Press Ctrl+C
```

**Kill all processes:**
```bash
pkill -f "bin/server"
pkill -f "bin/client"
```

## Next Steps

1. Review the project structure in `BUILD_GUIDE_LINUX.md`
2. Examine the source code in `src/server/` and `src/client/`
3. Check the protocol specifications in `requirements.txt`
4. Implement additional features as needed

Enjoy using the NU-Information Exchange System!
