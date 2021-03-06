#ifndef SRC_STRFILE_H_
#define SRC_STRFILE_H_

#include <regex.h>
#include <string>
#include <vector>

constexpr uint32_t Version = 2;
constexpr uint32_t FlagRandom = 0x1 << 0;   // randomized pointers
constexpr uint32_t FlagOrdered = 0x1 << 1;  // ordered pointers
constexpr uint32_t FlagRotated = 0x1 << 2;  // rot-13'd text


class Datfile {

    uint32_t version;              // version number
    uint32_t numstr;               // # of strings in file
    uint32_t longlen;              // length of longest string
    uint32_t shortlen;             // length of shortest string
    uint32_t flags;                // bit field for flags
    char stuff[4];                 // long aligned space
    std::vector<uint32_t> seekpts; // seek pointers

 public:
    void load(std::string const&);
    uint32_t start_of(int which) { return seekpts[which]; }
    uint32_t end_of(int which) { return seekpts[which + 1]; }
    bool is_rotated() { return (flags & FlagRotated) != 0; }
    int size() { return numstr; }
    int num_str() { return numstr; }
    char separator() { return stuff[0]; }
    uint32_t get_longlen() { return longlen; }
    uint32_t get_shortlen() { return shortlen; }
};


class Strfile {
    Datfile dat;       // strfile metadata

 public:
    std::string name;  // cookie file name
    float weight;      // weight of this file for random pick
    std::string path;  // path to strfile metadata file

    Strfile();
    Strfile& load(std::string const&, float);
    int print_one(uint32_t, bool, bool, bool);
    int print_matches(regex_t *, int, bool, bool);
    int num_str() { return dat.num_str(); }
    int longlen() { return dat.get_longlen(); }
    int shortlen() { return dat.get_shortlen(); }
};


#endif  // SRC_STRFILE_H_
