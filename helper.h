#ifndef TEXT_STATISTICS_HELPER
#define TEXT_STATISTICS_HELPER

#include <string>
#include <sstream>
#include <sys/stat.h>

namespace helper {

    void print_usage(char *pname) {
        std::cout << "\nUsage: " << pname << " <file>\n";
    }

    // Returns (in bytes) the size of a file.
    unsigned int get_file_size(const std::string &fname) {
        struct stat stat_buffer;
        int rc = stat(fname.c_str(), &stat_buffer);
        return rc == 0 ? stat_buffer.st_size : -1;
    }

    std::string to_string(int val) {
        static std::string str;
        static std::stringstream ss;
        ss << val;
        ss >> str;
        ss.clear();
        return str;
    }
}

#endif
