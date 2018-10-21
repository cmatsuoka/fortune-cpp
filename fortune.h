#ifndef FORTUNE_H_
#define FORTUNE_H_

#include <string>
#include <vector>
#include "strfile.h"


class Fortune {
    int slen;                  // short fortune length
    bool long_only;            // display only long fortunes
    bool short_only;           // display only short fortunes
    bool show_file;            // display cookie file names
    bool all_fortunes;         // allow offensive fortunes
    bool offend;               // choose only from offensive fortunes
    std::vector<Strfile> jars; // list of cookie files 
public:
    Fortune();
    Fortune& long_fortunes();
    Fortune& short_fortunes();
    Fortune& short_len(int);
};


#endif
