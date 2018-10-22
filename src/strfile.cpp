#include "strfile.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include "file_io.h"


void Datfile::load(std::string const& path)
{
    InputFile file(path);

    version = file.read32b();

    if (version > STRFILE_VERSION) {
        throw std::runtime_error("invalid data file version");
    }

    numstr = file.read32b();
    longlen = file.read32b();
    shortlen = file.read32b();
    flags = file.read32b();
    stuff = file.read32b();

    for (uint32_t i = 0; i < numstr; i++) {
        seekpts.push_back(file.read32b());
    }

    // if sorted or random, sort seek pointers
    if (flags & (STRFILE_FLAG_RANDOM | STRFILE_FLAG_ORDERED)) {
        std::sort(seekpts.begin(), seekpts.end());
    }
}


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
    std::string name("filename");

    // check if file exists
    // ...

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
    uint32_t start = dat.start_of(which);
    uint32_t size = dat.end_of(which) - start - 2;

    if ((long_only || size > slen) && (short_only || size <= slen)) {
        return 0;
    }

    InputFile file(path);
    file.seekg(start);

    // Read fortune string from file
    char *temp = new char[size + 1];
    file.read(temp, size);
    temp[size] = 0;

    std::cout << temp;
    delete [] temp;

    return size;
}
