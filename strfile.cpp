#include <iostream>
#include <cstring>
#include <cerrno>
#include <fstream>
#include <algorithm>
#include "strfile.h"


namespace {

    uint32_t read32b(std::istream& f)
    {
        char b[4];
        f.read(b, 4);
        if (f.eof()) {
            throw std::runtime_error("error reading file");
        }
        return (((uint32_t)(uint8_t)b[0]) << 24) |
               (((uint32_t)(uint8_t)b[1]) << 16) |
               (((uint32_t)(uint8_t)b[2]) << 8)  |
                 (uint32_t)(uint8_t)b[3];
           
    }

    bool file_exists(std::string const& name)
    {
        std::ifstream f(name.c_str());
        return f.good();
    }

    std::string error()
    {
        char *s = strerror(errno);
        return std::string(s);
    }

}


void Datfile::load(std::string const& path)
{
    std::ifstream file(path);
    if (file.fail()) {
        throw std::runtime_error(path + ": " + error());
    }

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


Strfile::Strfile() :
    name(""),
    weight(0.0),
    path("")
{
}

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

int Strfile::print_one(uint32_t slen, bool long_only, bool short_only, bool show_file)
{
    int which = rand() % dat.size();
    uint32_t start = dat.start_of(which);
    uint32_t size = dat.end_of(which) - start - 2;

    if ((long_only || size > slen) && (short_only || size <= slen)) {
        return 0;
    }

    std::ifstream file(path);
    file.seekg(start);

    // Read fortune string from file
    char *temp = new char[size + 1];
    file.read(temp, size);
    temp[size] = 0;

    std::cout << temp;
    delete temp;

    return size;
}
