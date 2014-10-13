#ifndef TEXT_STATISTICS_HELPER
#define TEXT_STATISTICS_HELPER

#include <string>
#include <sstream>
#include <sys/stat.h>

namespace helper
{

void print_usage(char *pname)
{
    std::cout << "\nUsage: " << pname << " <file>\n";
}

// Returns (in bytes) the size of a file.
int get_file_size(const std::string &fname)
{
    struct stat stat_buffer;
    int rc = stat(fname.c_str(), &stat_buffer);
    return rc == 0 ? stat_buffer.st_size : -1;
}

std::string to_string(unsigned long value)
{
    // The length of 2 ^ 64 - 1 is 20.
    // ULONG is usually 2 ^ 32 - 1, but the code sticks to 2 ^ 64 - 1 just in case a implementation eventually
    // has a 64-bit unsigned long.
    char buffer[20];
    // sprintf stores a formatted string into a C-string.
    std::sprintf(buffer, "%lu", value); // "lu" is used for unsigned long.
    return buffer;
}
}

#endif
