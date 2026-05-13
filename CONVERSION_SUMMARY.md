# Project Conversion Complete: Windows → Linux

## Summary of Changes

Your NU-Information Exchange System project has been successfully converted from **Windows (Winsock2)** to **Linux (POSIX sockets)** with a proper modular structure.

### ✅ What Was Done

#### 1. **Modular Architecture**
- **Separated concerns**: Server and client are now in separate modules
- **Reusable headers**: Common utilities in `include/common.h`
- **Clean structure**: Following standard C++ project layout

```
src/
├── server/
│   ├── server.h          (Server class definition)
│   ├── server.cpp        (Server implementation)
│   └── main_server.cpp   (Server entry point)
└── client/
    ├── client.h          (Client class definition)
    ├── client.cpp        (Client implementation)
    └── main_client.cpp   (Client entry point)

include/
└── common.h              (Shared utilities & constants)
```

#### 2. **Linux/POSIX Socket Implementation**

**Replaced Windows APIs:**
```cpp
// OLD (Windows)
#include <winsock2.h>
#include <ws2tcpip.h>
WSAStartup() / WSACleanup()

// NEW (Linux/POSIX)
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
close() instead of closesocket()
```

**Socket Functions Now POSIX-Compatible:**
- `socket()` - Create socket
- `bind()` - Bind to port
- `listen()` - Listen for connections
- `accept()` - Accept client connection
- `connect()` - Connect to server
- `send()` / `recv()` - TCP communication
- `sendto()` / `recvfrom()` - UDP communication
- `close()` - Close socket

#### 3. **Build System (Makefile)**
- Automatic compilation of all source files
- Object files organized in `build/` directory
- Executables in `bin/` directory
- Built with `-std=c++11 -pthread` flags
- Supports multiple targets: `build`, `clean`, `rebuild`, `run-server`, `run-client`

#### 4. **Documentation Created**

| File | Purpose |
|------|---------|
| `BUILD_GUIDE_LINUX.md` | Complete technical guide with architecture details |
| `QUICKSTART.md` | Quick reference for common commands |
| `WSL_SETUP.md` | WSL-specific setup instructions |
| `Makefile` | Build configuration |
| `setup.sh` | Automated setup script |
| `test.sh` | Testing script |

### 📁 Project Structure

```
University-Information-Exchange-System/
├── include/
│   └── common.h                    # Shared utilities, constants, helpers
├── src/
│   ├── server/
│   │   ├── server.h               # Server class header
│   │   ├── server.cpp             # Server implementation (TCP/UDP)
│   │   └── main_server.cpp        # Server main() entry point
│   └── client/
│       ├── client.h               # Client class header
│       ├── client.cpp             # Client implementation (TCP/UDP)
│       └── main_client.cpp        # Client main() entry point
├── Makefile                        # Build configuration
├── setup.sh                        # Setup & build script
├── test.sh                         # Testing script
├── BUILD_GUIDE_LINUX.md            # Detailed guide
├── QUICKSTART.md                   # Quick start
├── WSL_SETUP.md                    # WSL instructions
├── requirements.txt                # Project specifications
└── [generated at build time]
    ├── bin/
    │   ├── server                  # Server executable
    │   └── client                  # Client executable
    └── build/
        ├── server.o
        ├── main_server.o
        ├── client.o
        └── main_client.o
```

### 🔧 Building the Project

#### On WSL (Recommended for you):

```bash
# Install dependencies
sudo apt-get update
sudo apt-get install build-essential

# Navigate to project
cd /path/to/University-Information-Exchange-System

# Build
make all
# OR
./setup.sh
```

#### Compilation Details:
- **Compiler**: g++ with C++11 standard
- **Flags**: `-Wall -Wextra -pthread -O2`
- **Includes**: `include/` directory for headers
- **Output**: Binaries in `bin/` directory

### 🚀 Running the Application

**Terminal 1 - Start Server:**
```bash
make run-server
# OR
./bin/server
```

**Terminal 2 - Start Campus Client:**
```bash
make run-client CAMPUS_NAME=Lahore
# OR
./bin/client Lahore
```

**Terminal 3+ - Start Additional Clients:**
```bash
make run-client CAMPUS_NAME=Karachi
make run-client CAMPUS_NAME=Peshawar
```

### 🔌 Network Protocol (Unchanged)

#### TCP Port 9000 - Reliable Communication
```
AUTH|Campus:<Name>,Pass:<Password>
MSG|<SrcCampus>|<SrcDept>|<DstCampus>|<DstDept>|<Message>
BROADCAST|<Message>
```

#### UDP Port 9001 - Heartbeats & Broadcasts
```
HEARTBEAT|<CampusName>|<Timestamp>
```

### ✨ Key Features Implemented

✅ **TCP Socket Programming**
- Multiple concurrent client connections handled with threads
- Reliable message delivery between campuses
- Authentication on connection

✅ **UDP Socket Programming**
- Periodic heartbeat monitoring (every 10 seconds)
- System-wide broadcast announcements
- Connectionless, low-overhead communication

✅ **Multi-threading**
- Separate thread for each connected client
- Dedicated UDP listener thread
- Admin console in its own thread
- Message receive threads in clients

✅ **Thread Safety**
- Mutex protection for shared data structures
- Safe concurrent access to maps

✅ **Message Routing**
- Server receives messages and routes to destination campus
- Handles multiple simultaneous messages

✅ **Admin Features**
- List all connected campuses with status
- Broadcast messages to all connected clients
- Real-time heartbeat monitoring

### 📋 Campus Examples

All these work now on Linux/WSL:
```bash
./bin/client Lahore
./bin/client Karachi
./bin/client Peshawar
./bin/client Islamabad
./bin/client Multan
```

### 🛠 Available Make Commands

```bash
make                    # Build both server and client
make all               # Same as above
make clean             # Remove build artifacts
make rebuild           # Clean and rebuild
make run-server        # Build and run server
make run-client CAMPUS_NAME=Lahore   # Build and run client
make help              # Show all commands
```

### 🔍 Troubleshooting

**Port already in use:**
```bash
lsof -i :9000
kill -9 <PID>
```

**Rebuild from scratch:**
```bash
make clean && make all
```

**Missing dependencies:**
```bash
sudo apt-get install build-essential
```

### 📝 Key Differences from Original

| Aspect | Old (Windows) | New (Linux) |
|--------|---------------|------------|
| Socket headers | `<winsock2.h>` | `<sys/socket.h>` + others |
| Initialization | WSAStartup/Cleanup | None needed |
| Socket type | SOCKET | int |
| Close socket | closesocket() | close() |
| Threading | std::thread (same) | std::thread (same) |
| Compilation | MSVC/Visual Studio | g++ with Makefile |
| Time format | ctime_s() | ctime_r() |
| Project layout | Flat | Modular with structure |

### 📚 Documentation Files

1. **BUILD_GUIDE_LINUX.md** - Full technical documentation
2. **QUICKSTART.md** - 5-minute quick start guide
3. **WSL_SETUP.md** - WSL-specific setup and troubleshooting
4. **Makefile** - Build configuration with detailed comments
5. **setup.sh** - Automated setup script
6. **test.sh** - Automated testing script

### ✅ What's Been Verified

- ✅ All includes changed to POSIX socket headers
- ✅ All socket operations use Linux-compatible APIs
- ✅ Makefile builds successfully
- ✅ Proper separation into server and client modules
- ✅ Common utilities extracted to shared header
- ✅ Thread-safe implementation with mutexes
- ✅ Signal handlers for graceful shutdown
- ✅ Authentication and message routing preserved
- ✅ TCP and UDP functionality maintained
- ✅ Admin console features preserved

### 🎯 Next Steps

1. **Test the build:**
   ```bash
   cd /path/to/project
   make all
   ```

2. **Run on WSL:**
   ```bash
   make run-server        # Terminal 1
   make run-client CAMPUS_NAME=Lahore   # Terminal 2
   make run-client CAMPUS_NAME=Karachi  # Terminal 3
   ```

3. **Review the code:**
   - Check `src/server/server.cpp` for server logic
   - Check `src/client/client.cpp` for client logic
   - Review `include/common.h` for shared utilities

4. **Customize as needed:**
   - Add new features to the modules
   - Modify ports or protocols in `common.h`
   - Extend the admin console functionality

### 📞 Support Resources

- **Protocol Specs**: See `requirements.txt`
- **Linux Sockets**: Use Linux man pages (`man socket`, `man bind`, etc.)
- **C++ Threading**: Standard C++11 `<thread>` library
- **WSL Help**: See `WSL_SETUP.md`

---

**Status: ✅ Conversion Complete and Ready to Use**

The project is now fully Linux/POSIX compatible and ready to run on WSL!
