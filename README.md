# File Explorer Application (C++ | Linux)

### Objective
A **console-based File Explorer** built in **C++** that interfaces directly with the **Linux operating system** through system calls.  
It allows users to **navigate directories, manage files**, and perform core file operations via a custom shell interface.

This project was developed as part of the *Capstone Project* under **LinuxOS and LSP assignments**, following a 5-day implementation plan.

---

## Project Plan (5-Day Implementation)

| Day | Task | Status |
|-----|------|--------|
| **Day 1** | Design project structure and setup development environment. Implement basic file listing. | ✅ Done |
| **Day 2** | Add directory navigation — `cd`, `pwd`, and an interactive shell. | ✅ Done |
| **Day 3** | Implement file operations: `cat`, `mkdir`, `rm`, `cp`, `mv`. | ✅ Done |
| **Day 4** | Implement file search functionality using recursion (`find <pattern>`). | ✅ Done |
| **Day 5** | Add file permission management (`info <file>`), and improve UI (colors, `clear` command). | ✅ Done |

---

## Features

**Navigation**
- `ls` → List all files and directories in the current folder  
- `cd <dir>` → Change directory  
- `pwd` → Show current working directory  

**File Operations**
- `cat <file>` → Display file contents  
- `mkdir <name>` → Create a new directory  
- `rm <file>` → Delete a file  
- `cp <src> <dest>` → Copy a file  
- `mv <old> <new>` → Move or rename a file  

**Search & Info**
- `find <pattern>` → Recursively search files or directories  
- `info <file>` → Display size, permissions (rwx bits), and last modified time  

**Interface**
- `clear` → Clears the screen  
- `help` → Displays all available commands  
- `exit` → Exits the explorer  

**UI Enhancements**
- Color-coded output:
  - 🟦 Blue → Directories  
  - 🟩 Green → Success messages  
  - 🟥 Red → Errors  

---

## Concepts Used

- **C++17 STL**: `vector`, `string`, `ifstream`, `ofstream`
- **POSIX system calls**: `opendir`, `readdir`, `stat`, `mkdir`, `remove`, `rename`, `chdir`
- **Error handling** via `errno` and `strerror`
- **Recursive traversal** for searching
- **ANSI escape codes** for colored terminal output
- **Linux file permissions** via bitwise checks on `st_mode`

---

## How to Compile and Run

### Requirements
- Linux or **WSL (Windows Subsystem for Linux)**
- g++ compiler

### Steps
```bash
# Clone the repository
git clone https://github.com/<your-username>/file-explorer.git
cd file-explorer

# Build
mkdir -p build
g++ -std=c++17 -Wall -Wextra -O2 src/main.cpp -o build/file_explorer

# Run
./build/file_explorer
