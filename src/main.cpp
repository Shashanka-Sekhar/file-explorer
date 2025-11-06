#include <iostream>
#include <string>
#include <vector>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <cstdio>
#include <errno.h>
#include <cstring>
#include <ctime>

#define BLUE   "\033[1;34m"
#define WHITE  "\033[0m"
#define GREEN  "\033[1;32m"
#define RED    "\033[1;31m"
#define YELLOW "\033[1;33m"

struct DirEntry {
    std::string name;
    bool is_dir;
    long size;
};

std::vector<DirEntry> list_directory(const std::string &path, std::string &err) {
    std::vector<DirEntry> result;
    DIR *dir = opendir(path.c_str());
    if (!dir) {
        err = std::strerror(errno);
        return result;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name(entry->d_name);
        if (name == "." || name == "..") continue;

        bool is_dir = (entry->d_type == DT_DIR);
        long size = -1;
        struct stat st;
        std::string full = path + "/" + name;
        if (stat(full.c_str(), &st) == 0) {
            is_dir = S_ISDIR(st.st_mode);
            if (!is_dir) size = st.st_size;
        }
        result.push_back({name, is_dir, size});
    }
    closedir(dir);
    return result;
}

// ---------- Existing utility functions from Day 4 ----------
void print_directory_listing(const std::string &path);
void cat_file(const std::string &filename);
void make_directory(const std::string &dirname);
void remove_file(const std::string &filename);
void copy_file(const std::string &src, const std::string &dest);
void move_file(const std::string &oldname, const std::string &newname);
void print_help();

// ---------- New Day-5 Features ----------

// Recursive search
void find_files(const std::string &base_path, const std::string &pattern) {
    DIR *dir = opendir(base_path.c_str());
    if (!dir) return;
    struct dirent *entry;

    while ((entry = readdir(dir)) != nullptr) {
        std::string name(entry->d_name);
        if (name == "." || name == "..") continue;
        std::string full = base_path + "/" + name;

        if (name.find(pattern) != std::string::npos)
            std::cout << GREEN << full << WHITE << "\n";

        struct stat st;
        if (stat(full.c_str(), &st) == 0 && S_ISDIR(st.st_mode))
            find_files(full, pattern); // recursive call
    }
    closedir(dir);
}

// File information
void file_info(const std::string &filename) {
    struct stat st;
    if (stat(filename.c_str(), &st) != 0) {
        std::cerr << RED << "Error reading file info: " << std::strerror(errno) << WHITE << "\n";
        return;
    }

    std::cout << YELLOW << "\nFile: " << filename << WHITE << "\n";
    std::cout << "Size: " << st.st_size << " bytes\n";

    std::cout << "Permissions: ";
    std::cout << ((st.st_mode & S_IRUSR) ? "r" : "-");
    std::cout << ((st.st_mode & S_IWUSR) ? "w" : "-");
    std::cout << ((st.st_mode & S_IXUSR) ? "x" : "-");
    std::cout << ((st.st_mode & S_IRGRP) ? "r" : "-");
    std::cout << ((st.st_mode & S_IWGRP) ? "w" : "-");
    std::cout << ((st.st_mode & S_IXGRP) ? "x" : "-");
    std::cout << ((st.st_mode & S_IROTH) ? "r" : "-");
    std::cout << ((st.st_mode & S_IWOTH) ? "w" : "-");
    std::cout << ((st.st_mode & S_IXOTH) ? "x" : "-") << "\n";

    std::cout << "Last modified: " << std::ctime(&st.st_mtime);
}

// Clear screen
void clear_screen() {
    std::cout << "\033[2J\033[1;1H";
}


// ---------- Shell Runner ----------
void run_shell() {
    char cwd[1024];
    while (true) {
        if (!getcwd(cwd, sizeof(cwd))) {
            std::cerr << "Error getting current directory\n";
            break;
        }

        std::cout << "\n" << BLUE << cwd << WHITE << " > ";

        std::string cmd;
        std::getline(std::cin, cmd);

        if (cmd == "exit") {
            std::cout << GREEN << "Goodbye!\n" << WHITE;
            break;
        } else if (cmd == "pwd") {
            std::cout << cwd << "\n";
        } else if (cmd == "ls") {
            print_directory_listing(cwd);
        } else if (cmd.rfind("cd ", 0) == 0) {
            if (chdir(cmd.substr(3).c_str()) != 0)
                std::cerr << RED << "Error changing directory: " << std::strerror(errno) << WHITE << "\n";
        } else if (cmd.rfind("cat ", 0) == 0) {
            cat_file(cmd.substr(4));
        } else if (cmd.rfind("mkdir ", 0) == 0) {
            make_directory(cmd.substr(6));
        } else if (cmd.rfind("rm ", 0) == 0) {
            remove_file(cmd.substr(3));
        } else if (cmd.rfind("cp ", 0) == 0) {
            size_t space = cmd.find(' ', 3);
            if (space == std::string::npos)
                std::cerr << RED << "Usage: cp <src> <dest>\n" << WHITE;
            else
                copy_file(cmd.substr(3, space - 3), cmd.substr(space + 1));
        } else if (cmd.rfind("mv ", 0) == 0) {
            size_t space = cmd.find(' ', 3);
            if (space == std::string::npos)
                std::cerr << RED << "Usage: mv <old> <new>\n" << WHITE;
            else
                move_file(cmd.substr(3, space - 3), cmd.substr(space + 1));
        } else if (cmd.rfind("find ", 0) == 0) {
            find_files(cwd, cmd.substr(5));
        } else if (cmd.rfind("info ", 0) == 0) {
            file_info(cmd.substr(5));
        } else if (cmd == "clear") {
            clear_screen();
        } else if (cmd == "help") {
            print_help();
        } else if (cmd.empty()) {
            continue;
        } else {
            std::cout << RED << "Unknown command: " << cmd << "\nType 'help' for list of commands.\n" << WHITE;
        }
    }
}

// ---------- Previous helper implementations ----------
void print_directory_listing(const std::string &path) {
    std::string err;
    auto entries = list_directory(path, err);
    if (!err.empty()) {
        std::cerr << RED << "Error: " << err << WHITE << "\n";
        return;
    }
    for (const auto &e : entries) {
        if (e.is_dir)
            std::cout << BLUE << "[DIR]  " << e.name << WHITE << "\n";
        else
            std::cout << "       " << e.name << " (" << e.size << " bytes)\n";
    }
}
void cat_file(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << RED << "Error opening file: " << std::strerror(errno) << WHITE << "\n";
        return;
    }
    std::string line;
    while (std::getline(file, line))
        std::cout << line << "\n";
    file.close();
}
void make_directory(const std::string &dirname) {
    if (mkdir(dirname.c_str(), 0755) != 0)
        std::cerr << RED << "Error creating directory: " << std::strerror(errno) << WHITE << "\n";
    else
        std::cout << GREEN << "Directory created: " << dirname << WHITE << "\n";
}
void remove_file(const std::string &filename) {
    if (remove(filename.c_str()) != 0)
        std::cerr << RED << "Error deleting file: " << std::strerror(errno) << WHITE << "\n";
    else
        std::cout << GREEN << "File deleted: " << filename << WHITE << "\n";
}
void copy_file(const std::string &src, const std::string &dest) {
    std::ifstream in(src, std::ios::binary);
    std::ofstream out(dest, std::ios::binary);
    if (!in.is_open() || !out.is_open()) {
        std::cerr << RED << "Error copying files.\n" << WHITE;
        return;
    }
    out << in.rdbuf();
    std::cout << GREEN << "Copied " << src << " -> " << dest << WHITE << "\n";
}
void move_file(const std::string &oldname, const std::string &newname) {
    if (rename(oldname.c_str(), newname.c_str()) != 0)
        std::cerr << RED << "Error moving/renaming file: " << std::strerror(errno) << WHITE << "\n";
    else
        std::cout << GREEN << "Moved/Renamed: " << oldname << " -> " << newname << WHITE << "\n";
}
void print_help() {
    std::cout << "\n" << BLUE << "Available commands:\n" << WHITE;
    std::cout << "  ls                - List directory contents\n";
    std::cout << "  cd <dir>          - Change directory\n";
    std::cout << "  pwd               - Print current directory\n";
    std::cout << "  cat <file>        - Display file contents\n";
    std::cout << "  mkdir <dirname>   - Create a new directory\n";
    std::cout << "  rm <file>         - Delete a file\n";
    std::cout << "  cp <src> <dest>   - Copy a file\n";
    std::cout << "  mv <old> <new>    - Move or rename a file\n";
    std::cout << "  find <pattern>    - Recursively search files/folders\n";
    std::cout << "  info <file>       - Show file details\n";
    std::cout << "  clear             - Clear the screen\n";
    std::cout << "  help              - Show this menu\n";
    std::cout << "  exit              - Quit program\n\n";
}

int main() {
    std::cout << BLUE << "Welcome to Simple File Explorer v5!\n" << WHITE;
    std::cout << "Type 'help' to see available commands.\n";
    run_shell();
    return 0;
}
