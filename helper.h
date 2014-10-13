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

    std::string to_string(unsigned long value) {
        // The length of 2 ^ 64 - 1 is 20.
        char buffer[20];
        // "lu" is used for unsigned long.
        std::sprintf(buffer, "%lu", value);
        return buffer;
    }
}

#endif
