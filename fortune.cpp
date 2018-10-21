#include "fortune.h"


Fortune::Fortune() :
    slen(160),
    long_only(false),
    short_only(false),
    show_file(false),
    all_fortunes(false),
    offend(false),
    jars{}
{
}

Fortune& Fortune::long_fortunes()
{
    long_only = true;
    short_only = false;
    return *this;
}

Fortune& Fortune::short_fortunes()
{
    long_only = false;
    short_only = true;
    return *this;
}

Fortune& Fortune::short_len(int n)
{
    slen = n;
    return *this;
}
