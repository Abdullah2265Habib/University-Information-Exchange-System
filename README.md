# NU-Information Exchange System

A multi-campus network communication system that enables real-time information exchange between different FAST-NUCES campuses using C++ socket programming.

## 📋 Project Overview

This system simulates a Wide Area Network (WAN) connecting multiple FAST-NUCES campuses across Pakistan (Lahore, Karachi, Peshawar, Islamabad, Multan). It uses a **client-server architecture** with **hybrid TCP/UDP protocol** to manage inter-campus communication.

### Key Concept

- **Central Server** (Islamabad Campus): Acts as the hub managing all campus connections
- **Campus Clients** (Lahore, Karachi, Peshawar, etc.): Connect to central server and communicate with each other
- **Department Users**: Simulated users at each campus sending/receiving messages

## 🎯 What It Does

### Core Functionality

✅ **Multi-campus Communication**
- Send messages between different campuses and departments
- Automatic message routing through central server
- Real-time delivery

✅ **TCP (Port 9000) - Reliable Connection**
- Client authentication with the central server
- Direct campus-to-campus messaging
- Message acknowledgment

✅ **UDP (Port 9001) - Status Monitoring**
- Periodic heartbeat signals every 10 seconds
- System-wide announcements to all campuses
- Non-critical broadcast messages

✅ **Server Admin Features**
- Monitor all connected campuses and their status
- View last heartbeat timestamp for each campus
- Broadcast system-wide announcements

✅ **Thread-Safe Multi-Threading**
- Handle multiple client connections simultaneously
- Separate threads for TCP and UDP operations
- Mutex protection for shared data

## 🚀 Getting Started

### Prerequisites

**On WSL (Windows Subsystem for Linux):**
```bash
# Install build tools
sudo apt-get update
sudo apt-get install build-essential
```

**Verify installation:**
```bash
g++ --version
make --version
```

### Installation

1. **Clone or navigate to the project:**
   ```bash
   cd "University-Information-Exchange-System"
   ```

2. **Build the project:**
   ```bash
   make all
   ```
   
   Or use the setup script:
   ```bash
   chmod +x setup.sh
   ./setup.sh
   ```

### Running the System

#### Method 1: Using Make (Recommended)

**Terminal 1 - Start the Server:**
```bash
make run-server
```

**Terminal 2 - Start Campus Client #1:**
```bash
make run-client CAMPUS_NAME=Lahore
```

**Terminal 3 - Start Campus Client #2:**
```bash
make run-client CAMPUS_NAME=Karachi
```

**Terminal 4 - Start Campus Client #3 (Optional):**
```bash
make run-client CAMPUS_NAME=Peshawar
```

#### Method 2: Direct Execution

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

### Available Campuses

You can start clients for any of these campuses:
- Lahore
- Karachi
- Peshawar
- Islamabad
- Multan

## 📱 How to Use

### Sending a Message Between Campuses

1. Run a campus client (e.g., Lahore)
2. Select option **1** from the menu
3. Enter:
   - **Target Campus**: (e.g., Karachi)
   - **Target Department**: (e.g., Academics)
   - **Your Department**: (e.g., Admissions)
   - **Message**: (e.g., "Please send admission statistics")
4. Message is sent through server to destination campus

### Viewing Received Messages

1. Select option **2** from the client menu
2. View all messages received from other campuses and broadcasts

### Broadcasting from Server

1. In server console, select **2** from admin menu
2. Enter message to broadcast
3. All connected campuses receive the announcement

### Admin Monitoring

1. In server console, select **1** from admin menu
2. View all connected campuses with their TCP status and last heartbeat time

## 🏗️ Project Architecture

```
┌─────────────────────────────────────────────────────┐
│         NU-Information Exchange System              │
├─────────────────────────────────────────────────────┤
│                                                     │
│  ┌──────────────────┐        ┌──────────────────┐ │
│  │  Central Server  │        │  Admin Console   │ │
│  │  (Islamabad)     │◄──────►│  (Monitoring)    │ │
│  │                  │        │                  │ │
│  │ ├─ TCP Port 9000 │        └──────────────────┘ │
│  │ ├─ UDP Port 9001 │                             │
│  │ └─ Router        │                             │
│  └────┬─────────────┘                             │
│       │                                            │
│   ┌───┴───┬────────┬─────────┬──────────┐         │
│   │       │        │         │          │         │
│  ┌─▼─┐  ┌─▼─┐  ┌──▼─┐  ┌───▼──┐  ┌────▼─┐      │
│  │L1 │  │K1 │  │ P1 │  │ I1   │  │ M1  │      │
│  │   │  │   │  │    │  │      │  │     │      │
│  │TCP◄──┤TCP├──┤TCP │  │ TCP  │  │ TCP │      │
│  │UDP│  │UDP│  │UDP │  │ UDP  │  │ UDP │      │
│  └───┘  └───┘  └────┘  └──────┘  └─────┘      │
│  Lab1   Kar1   Pes1    Isl1     Mul1          │
│ Lahore Karachi Peshawar Islamabad Multan      │
│                                                │
└─────────────────────────────────────────────────────┘
```

## 📁 Project Structure

```
University-Information-Exchange-System/
├── include/
│   └── common.h                    # Shared constants & utilities
├── src/
│   ├── server/
│   │   ├── server.h                # Server class
│   │   ├── server.cpp              # Server implementation
│   │   └── main_server.cpp         # Server entry point
│   └── client/
│       ├── client.h                # Client class
│       ├── client.cpp              # Client implementation
│       └── main_client.cpp         # Client entry point
├── Makefile                        # Build configuration
├── setup.sh                        # Setup script
├── test.sh                         # Testing script
├── quick_commands.sh               # Quick reference
├── requirements.txt                # Project specifications
├── BUILD_GUIDE_LINUX.md            # Detailed guide
├── QUICKSTART.md                   # Quick start
├── WSL_SETUP.md                    # WSL instructions
├── CONVERSION_SUMMARY.md           # Conversion details
└── BEFORE_AFTER.md                 # Transformation details
```

## 🔧 Build Commands

```bash
make all                            # Build server and client
make clean                          # Remove build artifacts
make rebuild                        # Clean and rebuild
make run-server                     # Build and run server
make run-client CAMPUS_NAME=Lahore  # Build and run client
make help                           # Show all commands
```

## 🧪 Testing

```bash
# Automated test (starts server and clients)
chmod +x test.sh
./test.sh
```

## 🔌 Network Protocol

### TCP Protocol (Port 9000)

```
Authentication:
  AUTH|Campus:<Name>,Pass:<Password>

Message Format:
  MSG|<SrcCampus>|<SrcDept>|<DstCampus>|<DstDept>|<MessageText>

Broadcast:
  BROADCAST|<Message>
```

### UDP Protocol (Port 9001)

```
Heartbeat:
  HEARTBEAT|<CampusName>|<Timestamp>

Sent every 10 seconds for status monitoring
```

## 📊 Features Implemented

| Feature | Status | Details |
|---------|--------|---------|
| TCP Socket Programming | ✅ | Multiple concurrent connections |
| UDP Socket Programming | ✅ | Heartbeat & broadcast system |
| Multi-threading | ✅ | Thread per client + dedicated listener threads |
| Thread Safety | ✅ | Mutex protection for shared data |
| Message Routing | ✅ | Server routes between campuses |
| Authentication | ✅ | Campus name & password validation |
| Admin Console | ✅ | Monitor & broadcast functionality |
| Linux/POSIX Compatibility | ✅ | Runs on WSL, Linux, macOS |

## 🛠️ Troubleshooting

### Port Already in Use
```bash
lsof -i :9000    # Find process on port 9000
kill -9 <PID>    # Kill the process
```

### Build Errors
```bash
make clean
make all
```

### Can't Connect
- Ensure server is running first
- Check firewall allows ports 9000 and 9001
- Try restarting with: `pkill -f bin/server`

### Slow Performance on WSL
- Use WSL2 instead of WSL1: `wsl --set-version Ubuntu 2`
- Store files in WSL filesystem (`~`) not Windows (`/mnt/`)

## 📚 Documentation

- **[QUICKSTART.md](QUICKSTART.md)** - Quick start guide (5 minutes)
- **[WSL_SETUP.md](WSL_SETUP.md)** - WSL-specific setup instructions
- **[BUILD_GUIDE_LINUX.md](BUILD_GUIDE_LINUX.md)** - Complete technical guide
- **[CONVERSION_SUMMARY.md](CONVERSION_SUMMARY.md)** - Windows to Linux conversion
- **[BEFORE_AFTER.md](BEFORE_AFTER.md)** - Detailed transformation details

## 👥 Example Session

### Terminal 1 - Start Server
```
$ make run-server
[SERVER] TCP Server listening on port 9000
[SERVER] UDP Server listening on port 9001
[SERVER] Central Server started and ready to accept connections

=== Admin Menu ===
1. List Connected Campuses
2. Broadcast Announcement
3. Exit
Select option:
```

### Terminal 2 - Start Lahore Client
```
$ make run-client CAMPUS_NAME=Lahore
[CLIENT] Connected to Central Server
[CLIENT] Heartbeat thread started

=== Menu ===
1. Send a message to another campus
2. View received messages
3. Exit
Enter choice:
```

### Terminal 3 - Start Karachi Client
```
$ make run-client CAMPUS_NAME=Karachi
[CLIENT] Connected to Central Server
[CLIENT] Heartbeat thread started
```

### Send Message from Lahore to Karachi
```
Terminal 2:
Enter choice: 1
Enter target campus: Karachi
Enter target department: Academics
Enter your department: Admissions
Enter message: Please send admission statistics

[SENT] MSG|Lahore|Admissions|Karachi|Academics|Please send admission statistics

Terminal 3:
=== Menu ===
[TCP MESSAGE] MSG|Lahore|Admissions|Karachi|Academics|Please send admission statistics

Enter choice: 2
--- Received Messages ---
[TCP MESSAGE] MSG|Lahore|Admissions|Karachi|Academics|Please send admission statistics
```

## 🎓 Learning Outcomes

After completing this project, you will understand:

✅ **Socket Programming** - TCP and UDP socket implementation  
✅ **Client-Server Architecture** - Multi-client connection handling  
✅ **Network Protocols** - Message format design and implementation  
✅ **Multi-threading** - Thread creation and management  
✅ **Thread Synchronization** - Mutex usage for thread safety  
✅ **Message Routing** - Directing messages between endpoints  
✅ **Linux/POSIX APIs** - Platform-specific socket operations  
✅ **Project Structure** - Modular code organization  

## 📋 Requirements

- **OS**: Linux, WSL, or macOS
- **Compiler**: g++ with C++11 support
- **Build System**: make
- **Memory**: ~50MB per process
- **Network**: Ports 9000 (TCP) and 9001 (UDP)

## 📝 Authentication

Default credentials for each campus:
- **Username (Campus)**: Lahore, Karachi, Peshawar, Islamabad, Multan
- **Password**: `123` (or `NU-<CampusName>-123`)

Example: 
```
Campus: Lahore
Password: 123
```

## 🔄 Workflow

```
1. Start Central Server
   ↓
2. Start Multiple Campus Clients
   ↓
3. Clients authenticate with server
   ↓
4. Clients send periodic UDP heartbeats (every 10 sec)
   ↓
5. Users send messages through client interface
   ↓
6. Server routes messages to destination campus
   ↓
7. Destination client receives and displays message
   ↓
8. Server broadcasts announcements via UDP
   ↓
9. All campuses receive broadcasts
```

## 💡 Tips

- **Multiple Terminals**: Use different terminal tabs/panes for server and clients
- **VS Code**: Use integrated terminal with split panes
- **Testing**: Run `./test.sh` for automated testing
- **Monitoring**: Use server admin menu to track connected campuses
- **Messages**: View received messages anytime from client menu

## 🐛 Known Issues

- Heartbeats may occasionally show stale timestamps (display only)
- Admin menu updates every 500ms
- Maximum buffer size: 4096 bytes per message

## 🚀 Future Enhancements

- GUI interface using Qt/GTK
- Persistent message storage (database)
- End-to-end encryption
- Message delivery acknowledgment
- File transfer support
- User presence/availability status
- Group messaging
- Message history

## 📄 License

See [LICENSE](LICENSE) file for details.

## 👨‍💻 Development Notes

This project was converted from Windows (Winsock2) to Linux (POSIX sockets) with a modular architecture. See [CONVERSION_SUMMARY.md](CONVERSION_SUMMARY.md) for details.

---

**Ready to use!** Follow the [Getting Started](#getting-started) section above to get started.