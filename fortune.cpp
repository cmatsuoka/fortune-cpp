#include <iostream>
#include <random>
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


namespace {

    std::vector<PathSpec> add_fortune_file(std::vector<PathSpec>, std::string const& name, float val)
    {
        std::vector<PathSpec> list{};

        auto datname = name + ".dat";
        auto entry = std::make_tuple(name, val);
        list.push_back(entry);

        return list;
    }

}

void Fortune::load(std::string const& what, float val)
{
    std::vector<PathSpec> files{};

    files = add_fortune_file(files, what, val);

    if (files.size() <= 0) {
        throw std::runtime_error("No fortunes found");
    }

    for (auto f : files) {
        auto sf = new Strfile();
        jars.push_back(sf->load(std::get<0>(f), std::get<1>(f)));
    }
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

Strfile& Fortune::pick_jar()
{
    int num = (rand() % jars.size());
    return jars[num];
}

int Fortune::print()
{
    return pick_jar().print_one(slen, long_only, short_only, show_file);
}
