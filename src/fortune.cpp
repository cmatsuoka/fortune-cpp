#include "fortune.h"
#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include <random>
#include "file_io.h"


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

    std::vector<PathSpec> add_fortune_dir(std::vector<PathSpec>, std::string const& dirname, float val,
        bool all_fortunes, bool offend)
    {
        std::vector<PathSpec> list{};
        std::vector<std::string> names{};
        DIR *dir;
        struct dirent *entry;

        if ((dir = opendir(dirname.c_str())) != NULL) {
            while ((entry = readdir(dir)) != NULL) {
                auto fn = dirname + File::separator() + std::string(entry->d_name);
                if (fn.substr(fn.find_last_of(".") + 1) == "dat") {
                    // remove file extension
                    names.push_back(fn.substr(0, fn.find_last_of(".")));
                }
            }
            closedir(dir);
        }

        float num = names.size();
        for (auto name : names) {
            auto entry = std::make_tuple(name, val / num);
            list.push_back(entry);
        }

        return list;
    }

    std::vector<PathSpec> add_fortune_file(std::vector<PathSpec>, std::string const& name, float val)
    {
        std::vector<PathSpec> list{};

        if (File::exists(name + ".dat")) {
            auto entry = std::make_tuple(name, val);
            list.push_back(entry);
        } else {
            throw std::runtime_error(name + ": missing strfile data file");
        }

        return list;
    }

}  // namespace

void Fortune::load(std::string const& what, float val)
{
    std::vector<PathSpec> files{};

    files = add_fortune_dir(files, what, val, all_fortunes, offend);

    if (files.size() <= 0) {
        throw std::runtime_error("No fortunes found");
    }

    for (auto f : files) {
        Strfile sf{};
        jars.push_back(sf.load(std::get<0>(f), std::get<1>(f)));
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
    int num = rand() % jars.size();
    return jars[num];
}

int Fortune::print()
{
    return pick_jar().print_one(slen, long_only, short_only, show_file);
}
