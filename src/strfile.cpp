#include "strfile.h"
#include <string.h>
#include <ctype.h>
#include <regex.h>
#include <iostream>
#include <algorithm>
#include <memory>
#include "file_io.h"


namespace {

    template <typename Iter> void rot13(Iter a, const Iter& b) {
        for (; a != b; a++) {
            auto c = toupper(*a);
            if (c >= 'A' && c <= 'M') {
                *a += 13;
            } else if (c >= 'N' && c <= 'Z') {
                *a -= 13;
            }
        }
    }


}  // namespace


void Datfile::load(std::string const& path)
{
    InputFile file(path);

    version = file.read32b();

    if (version > Version) {
        throw std::runtime_error("invalid data file version");
    }

    numstr = file.read32b();
    longlen = file.read32b();
    shortlen = file.read32b();
    flags = file.read32b();
    file.read(stuff, 4);

    for (uint32_t i = 0; i <= numstr; i++) {
        seekpts.push_back(file.read32b());
    }

    // if sorted or random, sort seek pointers
    if (flags & (FlagRandom | FlagOrdered)) {
        std::sort(seekpts.begin(), seekpts.end());
    }
}


// Strfile

Strfile::Strfile() :
    name(""),
    weight(0.0),
    path("")
{
}

/**
 * Load string metadata from file.
 * @param path The file to load
 * @param weight The weight of this file in random pick
 * @return This object
 */
Strfile& Strfile::load(std::string const& path, float weight)
{
    std::string name(path.substr(path.find_last_of(File::separator()) + 1));

    // check if file exists
    if (!File::is_file(path)) {
        throw std::runtime_error(path + ": invalid file type");
    }

    std::string dat_path(path + ".dat");
    dat.load(dat_path);

    this->name = name;
    this->path = path;
    this->weight = weight;

    return *this;
}

/**
 * Print a random message from this file.
 * @param slen Short message threshold
 * @param long_only Print only long messages
 * @param short_only Print only short messages
 * @param show_file Display the file name
 * @return The size of the printed message
 */
int Strfile::print_one(uint32_t slen, bool long_only, bool short_only, bool show_file)
{
    int which = rand() % dat.size();
    auto start = dat.start_of(which);
    auto size = dat.end_of(which) - start - 2;

    if ((long_only || size > slen) && (short_only || size <= slen)) {
        return 0;
    }

    InputFile file(path);
    file.seekg(start);

    // Read fortune string from file
    // pre-C++17: specify the deleter
    std::shared_ptr<char> buffer(new char[size + 1], std::default_delete<char[]>());
    char *temp = buffer.get();

    file.read(temp, size);
    temp[size] = '\0';

    if (dat.is_rotated()) {
        rot13(temp, temp + strlen(temp));
    }

    if (show_file) {
        std::cout << "(" << name << ")\n" << dat.separator() << "\n";
    }

    std::cout << temp << std::flush;

    return size;
}

/**
 * Print all messages from this file matching a given regular expression.
 * @param re The regular expression to match
 * @param slen The short message threshold
 * @param long_only Match only long messages
 * @param short_only Match only short messages
 * @return The number of matched messages
 */
int Strfile::print_matches(regex_t *re, int slen, bool long_only, bool short_only)
{
    InputFile file(path);

    // pre-C++17: specify the deleter
    std::shared_ptr<char> buffer(new char[longlen() + 1], std::default_delete<char[]>());
    char *temp = buffer.get();
    int num_matches = 0;
    bool print_name = true;

    for (int i = 0; i < num_str(); i++) {
        auto start = dat.start_of(i);
        auto size = dat.end_of(i) - start - 2;

        if ((!long_only && size <= slen) || (!short_only && size > slen)) {
            file.seekg(start);
            file.read(temp, size);
            temp[size] = '\0';

            if (regexec(re, temp, 0, NULL, 0) == 0) {
                if (print_name) {
                    std::cerr << "(" << name << ")\n" << dat.separator() << std::endl;
                    print_name = false;
                }

                std::cout << temp << std::endl;
                num_matches++;
            }
        }
    }

    return num_matches;
}
