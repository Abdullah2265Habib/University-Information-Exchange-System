# WSL Setup Guide for NU-Information Exchange System

## Prerequisites on Windows

1. **Install WSL2** (Recommended)
   - Open PowerShell as Administrator
   - Run: `wsl --install`
   - Restart your computer
   - Choose Ubuntu distribution (or preferred Linux distro)

2. **Install VS Code with WSL Extension**
   - Download: https://code.visualstudio.com/
   - Install extension: "Remote - WSL" by Microsoft
   - Then open your project folder in WSL

## Setup on WSL Terminal

### Step 1: Open WSL Terminal

```bash
# From Windows PowerShell:
wsl

# Or from Windows Terminal:
# Select Ubuntu/your Linux distro from dropdown
```

### Step 2: Navigate to Project

```bash
# Example path - adjust as needed
cd /mnt/e/University\ Work/CN_LAB\ Project/University-Information-Exchange-System
# Or use shorter path if you set it up
cd ~/projects/University-Information-Exchange-System
```

### Step 3: Install Dependencies

```bash
# Update package manager
sudo apt-get update
sudo apt-get upgrade -y

# Install build tools
sudo apt-get install -y build-essential

# Verify g++ installation
g++ --version
```

### Step 4: Build Project

```bash
# Make setup script executable
chmod +x setup.sh

# Run setup (this builds everything)
./setup.sh
```

## Running on WSL

### Option A: Using VS Code (Easiest)

1. Open VS Code
2. Click "Remote Explorer" (left sidebar)
3. Select "WSL Targets"
4. Click the folder icon and navigate to the project
5. Open integrated terminal (Ctrl+`)
6. Run commands:
   ```bash
   make run-server    # Terminal 1
   make run-client CAMPUS_NAME=Lahore   # Terminal 2
   ```

### Option B: Using Multiple WSL Terminals

**Terminal 1 - Start Server:**
```bash
cd ~/projects/University-Information-Exchange-System
make run-server
```

**Terminal 2 - Start Client 1:**
```bash
cd ~/projects/University-Information-Exchange-System
make run-client CAMPUS_NAME=Lahore
```

**Terminal 3 - Start Client 2:**
```bash
cd ~/projects/University-Information-Exchange-System
make run-client CAMPUS_NAME=Karachi
```

### Option C: Manual Build and Run

```bash
# Build
make all

# Run server
./bin/server

# In another terminal, run client
./bin/client Lahore
```

## Accessing Files from Windows

Your project is located at:
```
\\wsl$\Ubuntu\home\<your-username>\projects\University-Information-Exchange-System
```

You can access it from:
- Windows Explorer (paste the path in address bar)
- VS Code Remote - WSL extension
- Any Windows text editor

## Networking on WSL

### Local Testing (on WSL only)
Everything works automatically on `localhost` and `127.0.0.1`

### Accessing from Windows While WSL Server Runs

1. Get WSL IP:
   ```bash
   hostname -I
   ```
   (Usually something like `172.x.x.x`)

2. From Windows cmd or PowerShell:
   ```bash
   # If you had built it for Windows
   client.exe <CampusName>
   ```

## Port Access

- **TCP Port 9000**: Server listening port (used automatically)
- **UDP Port 9001**: Heartbeat/broadcast port (used automatically)

These are accessible:
- From WSL processes automatically
- From Windows `localhost` (thanks to WSL networking)
- From other WSL distros on the same machine

## Performance Tips

1. **Store projects in WSL filesystem** (faster than /mnt/c or /mnt/e)
   ```bash
   # Best location for WSL performance
   ~/projects/
   ```

2. **Use WSL2** instead of WSL1 (much better performance)
   ```bash
   # Check version
   wsl --list --verbose
   
   # Convert to WSL2 if needed
   wsl --set-version Ubuntu 2
   ```

3. **Disable antivirus scanning** of `/mnt/` for faster access (optional)

## Troubleshooting

### "Command not found: make"
```bash
sudo apt-get install make
```

### "Permission denied" for scripts
```bash
chmod +x setup.sh
chmod +x test.sh
```

### "g++: command not found"
```bash
sudo apt-get install build-essential
```

### Ports already in use
```bash
# Find what's using port 9000
sudo lsof -i :9000

# Kill it
sudo kill -9 <PID>
```

### Slow file access from Windows path
```bash
# Use WSL home directory instead
cd ~
mkdir -p projects
cd projects
# Copy/clone project here instead of /mnt/
```

### Can't connect between Windows and WSL
```bash
# Check if server is listening
netstat -tlnp | grep -E '9000|9001'

# Try localhost or 127.0.0.1
# Should work automatically
```

## Recommended Setup

1. Install WSL2
2. Copy project to `~/projects/University-Information-Exchange-System`
3. Open in VS Code Remote - WSL
4. Use integrated terminal for all commands
5. Use split terminal panes for running server + multiple clients

## Quick Commands Reference

```bash
# Build
make all

# Clean
make clean

# Rebuild
make rebuild

# Run server
make run-server

# Run client
make run-client CAMPUS_NAME=Lahore

# Help
make help

# Run tests
./test.sh

# List connected campuses (requires server running)
# Use option 1 in server admin menu
```

## Next Steps

- Read `BUILD_GUIDE_LINUX.md` for detailed architecture
- Check `QUICKSTART.md` for testing procedures
- Review source code in `src/` directory
- Modify code as needed for your requirements

Happy coding!
