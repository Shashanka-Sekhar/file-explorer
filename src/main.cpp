#include <iostream>
#include <string>
#include <vector>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>

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

void print_directory_listing(const std::string &path) {
    std::string err;
    auto entries = list_directory(path, err);
    if (!err.empty()) {
        std::cerr << "Error: " << err << "\n";
        return;
    }

    for (const auto &e : entries) {
        if (e.is_dir)
            std::cout << "[DIR]  " << e.name << "\n";
        else
            std::cout << "       " << e.name << " (" << e.size << " bytes)\n";
    }
}

void run_shell() {
    char cwd[1024];
    while (true) {
        if (!getcwd(cwd, sizeof(cwd))) {
            std::cerr << "Error getting current directory\n";
            break;
        }

        std::cout << "\nCurrent Directory: " << cwd << "\n> ";

        std::string cmd;
        std::getline(std::cin, cmd);

        if (cmd == "exit") {
            std::cout << "Goodbye!\n";
            break;
        } else if (cmd == "pwd") {
            std::cout << cwd << "\n";
        } else if (cmd == "ls") {
            print_directory_listing(cwd);
        } else if (cmd.rfind("cd ", 0) == 0) {
            std::string dir = cmd.substr(3);
            if (chdir(dir.c_str()) != 0)
                std::cerr << "Error changing directory: " << std::strerror(errno) << "\n";
        } else if (cmd.empty()) {
            continue;
        } else {
            std::cout << "Unknown command: " << cmd << "\n";
            std::cout << "Available: ls, cd <dir>, pwd, exit\n";
        }
    }
}

int main() {
    std::cout << "Welcome to Simple File Explorer!\n";
    run_shell();
    return 0;
}
