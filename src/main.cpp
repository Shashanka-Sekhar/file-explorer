
#include <iostream>
#include <string>
#include <vector>
#include <dirent.h>   // opendir, readdir, closedir
#include <sys/stat.h> // stat
#include <errno.h>
#include <cstring>    // strerror

struct DirEntry {
    std::string name;
    bool is_dir;
    long size; // -1 if not available
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
        // skip "." and ".."
        if (name == "." || name == "..") continue;

        bool is_dir = (entry->d_type == DT_DIR);
        long size = -1;

        // Sometimes d_type might be DT_UNKNOWN; use stat() to be safe
        if (entry->d_type == DT_UNKNOWN || !is_dir) {
            struct stat st;
            std::string full = path + "/" + name;
            if (stat(full.c_str(), &st) == 0) {
                is_dir = S_ISDIR(st.st_mode);
                if (!is_dir) size = st.st_size;
            }
        }

        result.push_back({name, is_dir, size});
    }

    closedir(dir);
    return result;
}

int main(int argc, char **argv) {
    std::string path = ".";
    if (argc > 1) path = argv[1];

    std::string err;
    auto entries = list_directory(path, err);
    if (!err.empty()) {
        std::cerr << "Error opening directory '" << path << "': " << err << "\n";
        return 1;
    }

    std::cout << "Listing: " << path << "\n";
    for (const auto &e : entries) {
        if (e.is_dir)
            std::cout << "[DIR]  " << e.name << "\n";
        else
            std::cout << "       " << e.name << " (" << (e.size >= 0 ? std::to_string(e.size) + " bytes" : "size N/A") << ")\n";
    }
    return 0;
}
