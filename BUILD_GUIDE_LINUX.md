# Project Structure - Linux Version

## Directory Organization

```
University-Information-Exchange-System/
├── Makefile                          # Build configuration
├── setup.sh                          # Setup and build script
├── README.md                         # This file
├── requirements.txt                  # Project requirements/specifications
├── include/
│   └── common.h                      # Common utilities and constants
├── src/
│   ├── server/
│   │   ├── server.h                  # Server class header
│   │   ├── server.cpp                # Server implementation
│   │   └── main_server.cpp           # Server entry point
│   └── client/
│       ├── client.h                  # Client class header
│       ├── client.cpp                # Client implementation
│       └── main_client.cpp           # Client entry point
├── bin/                              # Compiled executables (generated)
│   ├── server                        # Central Server executable
│   └── client                        # Campus Client executable
└── build/                            # Object files (generated)
```

## Building the Project

### Prerequisites
- Linux or WSL (Windows Subsystem for Linux)
- g++ compiler (C++11 or later)
- POSIX-compatible system

### Build Steps

#### Option 1: Using the setup script
```bash
chmod +x setup.sh
./setup.sh
```

#### Option 2: Using make directly
```bash
# Build both server and client
make all

# Or rebuild from scratch
make rebuild

# Clean build artifacts
make clean
```

### Build Commands
```bash
make                          # Build server and client
make all                      # Same as above
make rebuild                  # Clean and rebuild
make clean                    # Remove build artifacts
make help                     # Show all available commands
```

## Running the Application

### Start the Central Server

```bash
./bin/server
```

The server will:
- Listen on TCP port 9000 for client connections
- Listen on UDP port 9001 for heartbeats and broadcasts
- Display a console menu for admin operations

### Start Campus Clients

In separate terminals:

```bash
# Terminal 2
./bin/client Lahore

# Terminal 3
./bin/client Karachi

# Terminal 4
./bin/client Peshawar

# Terminal 5
./bin/client Islamabad

# Terminal 6 (optional)
./bin/client Multan
```

Or use the make commands:
```bash
make run-server                           # Start server
make run-client CAMPUS_NAME=Lahore       # Start Lahore client
make run-client CAMPUS_NAME=Karachi      # Start Karachi client
```

## System Architecture

### Central Server
- Manages multiple campus clients via TCP
- Receives and routes messages between campuses
- Handles UDP heartbeats for monitoring
- Broadcasts system-wide announcements
- Runs an admin console for server management

### Campus Client
- Connects to the Central Server via TCP
- Authenticates with credentials
- Sends/receives messages from other campuses
- Sends periodic UDP heartbeats (every 10 seconds)
- Receives UDP broadcasts from the server
- Provides a text-based console interface

### Protocol Details

#### TCP (Port 9000) - Reliable Communication
- **Authentication**: `AUTH|Campus:<Name>,Pass:<Password>`
- **Message Format**: `MSG|<SrcCampus>|<SrcDept>|<DstCampus>|<DstDept>|<MessageText>`
- **Broadcast**: `BROADCAST|<Message>`

#### UDP (Port 9001) - Heartbeats & Broadcasts
- **Heartbeat**: `HEARTBEAT|<CampusName>|<Timestamp>`

## Features Implemented

✅ TCP Socket Programming
- Multiple concurrent client connections
- Thread-based client handling
- Reliable message delivery

✅ UDP Socket Programming
- Periodic heartbeat monitoring
- System-wide announcements
- Connectionless communication

✅ Multi-threading
- Separate threads for TCP connections
- Separate thread for UDP listener
- Admin console in dedicated thread
- Message receive threads in clients

✅ Thread Safety
- Mutex protection for shared data structures
- Safe concurrent access to client maps

✅ Message Routing
- Server routes messages between campuses
- Handles authentication

✅ Admin Features
- List connected campuses
- Broadcast messages to all clients
- View last heartbeat timestamps

## Authentication

Each campus authenticates with:
- **Campus Name**: Used as identifier
- **Password**: Default is "123" or "NU-<CampusName>-123"

Example:
```
Campus: Lahore
Password: 123 (or NU-Lahore-123)
```

## Testing the System

1. **Start the Server**
   ```bash
   make run-server
   ```

2. **Start Multiple Clients** (in different terminals)
   ```bash
   make run-client CAMPUS_NAME=Lahore
   make run-client CAMPUS_NAME=Karachi
   make run-client CAMPUS_NAME=Peshawar
   ```

3. **Send Messages**
   - Use option 1 in the client menu
   - Enter target campus, department, and message

4. **Monitor Status**
   - Use the admin menu (option 1) in server to list connected campuses
   - Use option 2 to broadcast announcements

## Troubleshooting

### Port Already in Use
If you get "Address already in use" error:
```bash
# Kill processes using the ports
sudo lsof -i :9000
sudo lsof -i :9001
sudo kill -9 <PID>
```

### Build Errors
```bash
# Make sure g++ is installed
sudo apt-get install build-essential

# Clean and rebuild
make clean
make all
```

### Connection Issues
- Ensure the server is running before starting clients
- Check that ports 9000 and 9001 are not blocked by firewall
- Verify server is listening: `netstat -tlnp | grep -E '9000|9001'`

## Linux/POSIX Socket API Used

- `socket()` - Create sockets
- `bind()` - Bind sockets to ports
- `listen()` - Listen for connections
- `accept()` - Accept client connections
- `connect()` - Connect to server
- `send()` / `recv()` - TCP data transfer
- `sendto()` / `recvfrom()` - UDP data transfer
- `close()` - Close sockets

## Key Differences from Windows Version

1. **Headers**: Uses `<sys/socket.h>`, `<netinet/in.h>`, `<arpa/inet.h>` instead of Winsock2
2. **Initialization**: No WSAStartup/WSACleanup needed
3. **Socket Operations**: Compatible with standard POSIX API
4. **Compilation**: Uses g++ with pthread support instead of MSVC
5. **Thread Creation**: Uses `std::thread` (works on both platforms)

## Notes for WSL Users

- This code runs on WSL1 and WSL2
- For networking between Windows host and WSL, use `localhost` or `127.0.0.1`
- To access from Windows cmd while WSL server is running:
  ```bash
  # Get WSL IP
  hostname -I
  # Or run client from Windows (if you have the Windows executable)
  ```

## Future Enhancements

- GUI interface using Qt or GTK
- Database for persistent message storage
- User authentication with encryption
- Message acknowledgment system
- File transfer support
- Presence/availability status
- Group messaging

## License

See LICENSE file in the repository.

## Support

For issues or questions, refer to the requirements.txt for project specifications.
