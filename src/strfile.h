#ifndef SRC_STRFILE_H_
#define SRC_STRFILE_H_

#define STRFILE_VERSION      2
#define STRFILE_FLAG_RANDOM  (0x1 << 0)  // randomized pointers
#define STRFILE_FLAG_ORDERED (0x1 << 1)  // ordered pointers
#define STRFILE_FLAG_ROTATED (0x1 << 2)  // rot-13'd text

#include <string>
#include <vector>


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
    bool is_rotated() { return (flags & STRFILE_FLAG_ROTATED) != 0; }
    int size() { return numstr; }
    char separator() { return stuff[0]; }
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
};


#endif  // SRC_STRFILE_H_
