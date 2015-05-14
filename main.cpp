// This program reads a text file and generates some statistics about it.
// Started by Bernardo Sulzbach on 11/10/2014.

#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <vector>
#include <map>

#include "helper.h"

const std::string HELP_FLAG = "-h";
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
        file_info["SIZE"] = std::to_string(helper::get_file_size(filename));
    }

    // Finds the most frequent ordered pair (e.g. "New York") of words in the
    // text.
    // TODO: ignore case.
    // TODO: consider respecting punctuation: "New. York" would not be a pair.
    void find_most_frequent_ordered_pair() {
        std::unordered_map<std::string, unsigned> frequencies;
        std::ifstream istream = get_ifstream();
        if (!istream.is_open()) {
            return;
        }
        // The two last words read from the file
        std::string a;
        std::string b;
        // Read the first word.
        istream >> a;
        while (istream >> b) {
            std::string ordered_pair = a + " " + b;
            auto iter = frequencies.find(ordered_pair);
            if (iter != frequencies.end()) {
                iter->second++;
            }
            else {
                frequencies.insert(std::make_pair(ordered_pair, 1));
            }
            a = b;
        }
        std::pair<std::string, unsigned> maximum;
        for (std::pair<std::string, unsigned> pair : frequencies) {
            if (pair.second > maximum.second) {
                maximum = pair;
            }
        }
        stat_map["MOST_FREQUENT_ORDERED_PAIR"] = maximum.first;
        stat_map["MOST_FREQUENT_ORDERED_PAIR_FREQUENCY"] = std::to_string(maximum.second);
    }

    void update() {
        // Set begin time.
        auto start_time = std::chrono::high_resolution_clock::now();

        std::ifstream istream = get_ifstream();
        if (!istream.is_open()) {
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
            for (auto i = line.begin(); i != line.end(); ++i) {
                if (iswalpha(*i)) {
                    // Don't increment alpha now (sum lower and upper instead).
                    if (iswupper(*i)) {
                        upper++;
                    }
                    else {
                        lower++;
                    }
                    // Found a character.
                    // All characters are considered to be part of a word, 
                    // so set in_a_word to true.
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

        stat_map["LINES"] = std::to_string(lines);
        stat_map["CHARACTERS"] = std::to_string(chars);
        stat_map["WORDS"] = std::to_string(words);

        stat_map["SPACES"] = std::to_string(space);

        stat_map["PUNCTUATION"] = std::to_string(punct);

        stat_map["DIGITS"] = std::to_string(digit);

        stat_map["LETTERS"] = std::to_string(alpha);
        stat_map["UPPERCASE"] = std::to_string(upper);
        stat_map["LOWERCASE"] = std::to_string(lower);

        // Just stop timing at the end of the update.
        auto now = std::chrono::high_resolution_clock::now();
        auto dur = std::chrono::duration<double, std::milli>(now - start_time);
        last_update.update_duration = dur;
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

private:
    // Attempts to save the statistics to a text file.
    // Returns true if successful, false otherwise.
    bool save(const std::string &filename) {
        std::ofstream ostream(filename, std::ios_base::out);

        if (!ostream.is_open()) {
            std::cerr << COULD_NOT_OPEN << std::endl;
            return false;
        }

        // Write the header.
        ostream << generate_output_header();

        // Add a separator.
        ostream << helper::separator << std::endl;

        if (stat_map.size() == 0) {
            ostream << COULD_NOT_OPEN << std::endl;
        }
        else {
            // Save all entries of stat_map in the format "key": "value"
            for (auto i = stat_map.begin(); i != stat_map.end(); ++i) {
                ostream << i->first << ": " << i->second << std::endl;
            }
        }

        // Add a separator.
        ostream << helper::separator << std::endl;

        // Write update method logs.
        ostream << "Took " << (int)last_update.update_duration.count() << " ms." << std::endl;
        return true;
    }

    // Generates a string that indicates the file's name and size.
    inline const std::string generate_output_header() {
        return file_info["NAME"] + "\n" + file_info["SIZE"] + " bytes.\n";
    }

    // Tries to get an ifstream from the provided filename.
    // This function will print an error to cerr if the file could not be open.
    std::ifstream get_ifstream() {
        // Get the name of the file and try to open it.
        std::string name = file_info["NAME"];
        std::ifstream istream;
        istream.open(name.c_str());
        if (!istream.is_open()) {
            std::cerr << COULD_NOT_OPEN << std::endl;
        }
        return istream;
    }

};

enum flag {
    MOST_FREQUENT_ORDERED_PAIR,
    UNKNOWN
};

// Checks if a string starts with two dashes.
bool has_option_flag_format(std::string arg) {
    return arg.size() >= 2 && arg.substr(0, 2) == "--";
}

// Converts a string to a flag.
flag to_flag(std::string arg) {
    if (arg == "--most-frequent-ordered-pair") {
        return MOST_FREQUENT_ORDERED_PAIR;
    }
    return UNKNOWN;
}

int main(int argc, char *argv[]) {
    std::vector<flag> passed_flags;
    std::vector<std::string> files;
    if (argc == 1) {
        helper::print_usage(argv[0]);
    }
    else if (argc > 1 && argv[1] == HELP_FLAG) {
        helper::print_help();
    }
    else {
        for (int i = 1; i < argc; ++i) {
            std::string argument = argv[i];
            if (has_option_flag_format(argument)) {
                passed_flags.push_back(to_flag(argument));
            }
            else {
                files.push_back(argv[i]);
            }
        }
    }
    for (std::string file : files) {
        Statistics stats(file);
        stats.update();
        if (helper::in_vector(MOST_FREQUENT_ORDERED_PAIR, passed_flags)) {
            stats.find_most_frequent_ordered_pair();
        }
        stats.save();
    }
    return 0;
}
