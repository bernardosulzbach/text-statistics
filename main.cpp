// This program reads a text file and generates some statistics about it.
// Started by Bernardo Sulzbach on 11/10/2014.

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <map>

#include "helper.h"

const std::string INPUT_DIR = "input/";
const std::string COULD_NOT_OPEN = "Could not open the file.";

struct UpdateLog {
    std::chrono::duration<double, std::milli> update_duration;
};

class Statistics {
public:
    UpdateLog last_update;
    std::map<std::string, std::string> file_info;
    std::map<std::string, std::string> stat_map;
    std::map<std::string, std::string> update_data;

    Statistics(const std::string &filename) {
        file_info["NAME"] = filename;
        file_info["SIZE"] = helper::to_string(helper::get_file_size(filename));
    }

    void update() {
        // Set begin time.
        auto update_begin = std::chrono::high_resolution_clock::now();

        // Get the name of the file and try to open it.
        std::string name = file_info["NAME"];
        std::ifstream istream;
        istream.open(name.c_str());

        if (!istream.is_open()) {
            std::cerr << COULD_NOT_OPEN << std::endl;
            return;
        }

        // Count some basic values.
        std::string line;

        bool in_a_word = false;

        unsigned int lines = 0;
        unsigned int words = 0;
        unsigned int chars = 0;
        unsigned int space = 0;

        unsigned int punct = 0;

        unsigned int digit = 0;

        unsigned int alpha = 0;
        unsigned int upper = 0;
        unsigned int lower = 0;

        unsigned int other = 0;

        while (getline(istream, line)) {
            lines++;
            chars += line.length();
            for (std::string::const_iterator i = line.begin(); i != line.end(); ++i) {
                if (iswalpha(*i)) {
                    // Do not increment alpha now.
                    // (I suppose) it is faster to just obtain alpha from the sum of lower and upper afterwards.
                    if (iswupper(*i)) {
                        upper++;
                    }
                    else {
                        lower++;
                    }
                    // Hit a character, all characters are considered to be part of a word, so we set in_a_word to true.
                    in_a_word = true;
                }
                else if (iswspace(*i)) {
                    space++;
                    // A word may have ended in this character.
                    if (in_a_word) {
                        words++;
                        in_a_word = false;
                    }
                }
                else if (iswpunct(*i)) {
                    punct++;
                    // A word may have ended in this character.
                    if (in_a_word) {
                        words++;
                        in_a_word = false;
                    }
                }
                else if (iswdigit(*i)) {
                    digit++;
                }
                else {
                    other++;
                }
            }
        }

        // Evaluate alpha here.
        alpha = upper + lower;

        stat_map["LINES"] = helper::to_string(lines);
        stat_map["CHARACTERS"] = helper::to_string(chars);
        stat_map["WORDS"] = helper::to_string(words);

        stat_map["SPACES"] = helper::to_string(space);

        stat_map["PUNCTUATION"] = helper::to_string(punct);

        stat_map["DIGITS"] = helper::to_string(digit);

        stat_map["LETTERS"] = helper::to_string(alpha);
        stat_map["UPPERCASE"] = helper::to_string(upper);
        stat_map["LOWERCASE"] = helper::to_string(lower);

        // Just stop timing at the end of the update.
        auto now = std::chrono::high_resolution_clock::now();
        last_update.update_duration = std::chrono::duration<double, std::milli>(now - update_begin);
    }

    // This is the method that should be invoked to save the statistics.
    // Returns true if successful, false otherwise.
    bool save() {
        std::string name = file_info["NAME"];

        // Check if there is a dot in the string.
        std::string::size_type last_dot = name.find_last_of('.');
        if (last_dot != std::string::npos) {
            name.insert(last_dot, "_stats");
        }
        else {
            name.append("_stats");
        }

        return save(name);
    }

    // Attempts to save the statistics to a text file.
    // Returns true if successful, false otherwise.
    bool save(const std::string &filename) {
        std::ofstream ostream(filename, std::ios_base::out);

        if (!ostream.is_open()) {
            std::cerr << COULD_NOT_OPEN << std::endl;
            return false;
        }

        // Write the header.
        ostream << file_info["NAME"] << std::endl;
        ostream << file_info["SIZE"] << " bytes." << std::endl;

        // Add a separator.
        std::string separator(80, '-');
        ostream << separator << std::endl;

        if (stat_map.size() == 0) {
            ostream << COULD_NOT_OPEN << std::endl;
        }
        else {
            // Save all entries of stat_map in the format "key": "value"
            for (std::map<std::string, std::string>::const_iterator i = stat_map.begin(); i != stat_map.end(); ++i) {
                ostream << i->first << ": " << i->second << std::endl;
            }
        }

        // Add a separator.
        ostream << separator << std::endl;

        // Write update method logs.
        ostream << "Took " << (int)last_update.update_duration.count() << " ms." << std::endl;
        return true;
    }

};

int main(int argc, char *argv[]) {

    if (argc == 1) {
        helper::print_usage(argv[0]);
        return -1;
    }
    else {
        for (int i = 1; i < argc; ++i) {
            Statistics stats(argv[i]);
            stats.update();
            stats.save();
        }
    }
    return 0;
}
