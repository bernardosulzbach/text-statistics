#ifndef TEXT_STATISTICS_HELPER
#define TEXT_STATISTICS_HELPER

#include <string>
#include <sstream>
#include <sys/stat.h>


namespace helper {
    const std::string HELP_TEXT =
        "Currently, the following flags (a.k.a. options) are implemented:\n"
        "--most-frequent-ordered-pair  - calculates the most frequent word pair.";

    // A line separator.
    const std::string separator = std::string(80, '-');

    // Prints the usage of this program, given its name.
    void print_usage(char *pname) {
        std::cout << "Usage:\n\t" << pname << " <file> " << "[OPTIONS]", "\n";
    }

    void print_help() {
        std::cout << HELP_TEXT << "\n";
    }

    // Verifies if an object is in a vector.
    template <typename T>
    bool in_vector(T t, std::vector<T> v) {
        return std::find(v.begin(), v.end(), t) != v.end();
    }

    // Returns (in bytes) the size of a file.
    int get_file_size(const std::string &fname) {
        struct stat stat_buffer;
        int rc = stat(fname.c_str(), &stat_buffer);
        return rc == 0 ? stat_buffer.st_size : -1;
    }
}
#endif