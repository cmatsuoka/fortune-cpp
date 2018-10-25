#ifndef SRC_FORTUNE_H_
#define SRC_FORTUNE_H_

#include <string>
#include <vector>
#include <tuple>
#include "strfile.h"


class Fortune {
    int slen;                  // short fortune length
    bool long_only;            // display only long fortunes
    bool short_only;           // display only short fortunes
    bool show_file;            // display cookie file names
    bool all_fortunes;         // allow offensive fortunes
    bool offend;               // choose only from offensive fortunes
    std::string fortune_dir;   // fortune directory
    std::vector<Strfile *> jars; // list of cookie files
 public:
    Fortune(std::string);
    ~Fortune();
    void load(std::string const&, float);
    Fortune& all();
    Fortune& offensive();
    Fortune& long_fortunes();
    Fortune& short_fortunes();
    Fortune& show_filename();
    Fortune& short_len(int);
    Fortune& equal_size();
    Fortune& normalize_weights();
    Strfile *pick_jar();
    int print();
    void search(std::string, bool = false);
    void print_weights();
    std::string const& path() { return fortune_dir; }
};


using PathSpec = std::tuple<std::string, float>;

#endif  // SRC_FORTUNE_H_
