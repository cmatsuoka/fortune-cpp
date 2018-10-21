
#include <iostream>
#include <fstream>
#include <algorithm>
#include "strfile.h"


namespace {

    uint32_t read32b(std::istream& f)
    {
        char b[4];
        f.read(b, 4);
        return (((uint32_t)b[0]) << 24) |
               (((uint32_t)b[1]) << 16) | 
               (((uint32_t)b[2]) << 8)  |
                 (uint32_t)b[3];
           
    }

}


void Datfile::load(std::string const& path)
{
    std::ifstream file;
    file.exceptions(std::ifstream::badbit);

    file.open(path);

    version = read32b(file);

    if (version > STRFILE_VERSION) {
        throw std::runtime_error("invalid data file version");
    }

    numstr = read32b(file);
    longlen = read32b(file);
    shortlen = read32b(file);
    flags = read32b(file);
    stuff = read32b(file);

    for (uint32_t i = 0; i < numstr; i++) {
        seekpts.push_back(read32b(file));
    }

    // if sorted or random, sort seek pointers
    if (flags & (STRFILE_FLAG_RANDOM | STRFILE_FLAG_ORDERED)) {
        std::sort(seekpts.begin(), seekpts.end());
    }

}
