#include "fortune.h"
#include <sys/types.h>
#include <dirent.h>
#include <regex.h>
#include <iostream>
#include <random>
#include "file_io.h"


namespace {

    bool ends_with(std::string const &s, std::string const &ending) {
        if (s.length() < ending.length()) {
            return false;
        }
        return s.compare(s.length() - ending.length(), ending.length(), ending) == 0;
    }

    std::vector<PathSpec> add_fortune_dir(std::vector<PathSpec>, std::string const& path, float val,
        bool all_fortunes, bool offend)
    {
        std::vector<PathSpec> list{};
        std::vector<std::string> names{};
        DIR *dir;
        struct dirent *entry;

        if ((dir = opendir(path.c_str())) != NULL) {
            while ((entry = readdir(dir)) != NULL) {
                auto fn = path + File::separator() + std::string(entry->d_name);
                if (ends_with(fn, ".dat")) {
                    // remove file extension
                    auto name = fn.substr(0, fn.find_last_of("."));
                    if (all_fortunes || !(offend ^ ends_with(name, "-o"))) {
                        names.push_back(name);
                    }
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

    std::vector<PathSpec> add_fortune_file(std::vector<PathSpec>, std::string const& path, float val)
    {
        std::vector<PathSpec> list{};

        if (File::is_file(path + ".dat", false)) {
            auto entry = std::make_tuple(path, val);
            list.push_back(entry);
        } else {
            throw std::runtime_error(path + ": missing strfile data file");
        }

        return list;
    }

}  // namespace


Fortune::Fortune(std::string path) :
    slen(160),
    long_only(false),
    short_only(false),
    show_file(false),
    all_fortunes(false),
    offend(false),
    fortune_dir(path),
    jars{}
{
}

Fortune::~Fortune()
{
    for (int i = 0; i < jars.size(); i++) {
        delete jars[i];
    }
}

/**
 * Load cookie files metadata.
 * @param what File or directory to load
 * @param val Weight in random pick
 */
void Fortune::load(std::string const& what, float val)
{
    std::vector<PathSpec> files{};

    if (File::is_directory(what)) {
        files = add_fortune_dir(files, what, val, all_fortunes, offend);
    } else if (File::is_file(what)) {
        files = add_fortune_file(files, what, val);
    } else {
        auto fullname = path() + File::separator() + what;
        if (File::is_file(fullname), true) {
            files = add_fortune_file(files, fullname, val);
        }
    }

    if (files.size() <= 0) {
        throw std::runtime_error("No fortunes found");
    }

    for (auto f : files) {
        auto sf = new Strfile();
        jars.push_back(&(sf->load(std::get<0>(f), std::get<1>(f))));
    }
}

/**
 * Select only long messages.
 * @return This fortune object
 */
Fortune& Fortune::long_fortunes()
{
    long_only = true;
    short_only = false;

    return *this;
}

/**
 * Select only short messages.
 * @return This fortune object
 */
Fortune& Fortune::short_fortunes()
{
    long_only = false;
    short_only = true;
    return *this;
}

/**
 * Set the short message threshold.
 * @param n The new message threshold
 * @return This fortune object
 */
Fortune& Fortune::short_len(int n)
{
    slen = n;
    return *this;
}

/**
 * Set all weights to the same value.
 * @return This fortune object
 */
Fortune& Fortune::equal_size()
{
    for (auto cf : jars) {
        cf->weight = 1.0;
    }
    return *this;
}

/**
 * Show the file where the fortune came from
 * @return This fortune object
 */
Fortune& Fortune::show_filename()
{
    show_file = true;
    return *this;
}

/**
 * Allow both offensive and not offensive fortunes.
 * @return This fortune object
 */
Fortune& Fortune::all()
{
    all_fortunes = true;
    offend = false;
    return *this;
}

/**
 * Select only from offensive fortunes.
 * @return This fortune object
 */
Fortune& Fortune::offensive()
{
    all_fortunes = false;
    offend = true;
    return *this;
}

/**
 * Normalize weights to totalize 100%.
 * @return This fortune object
 */
Fortune& Fortune::normalize_weights()
{
    float total = 100.0f;

    for (auto cf : jars) {
        if (cf->weight > 0.0f) {
            total -= cf->weight;
        }
    }

    if (total < 0.0f) {
        throw std::runtime_error("percentages must be <= 100");
    }

    float w = 0.0f;
    for (auto cf : jars) {
        if (cf->weight < 0.0) {
            w += cf->num_str();
        }
    }
    for (auto cf : jars) {
        if (cf->weight < 0.0) {
            cf->weight = total * cf->num_str() / w;
        }
    }

    return *this;
}

/**
 * Choose a random cookie file weighted by its number of strings. (FIXME: not yet)
 * @return The cookie jar string file
 */
Strfile *Fortune::pick_jar()
{
    int num = rand() % jars.size();
    return jars[num];
}

/**
 * Get a random string from a random cookie file.
 * @return The size of the printed message
 */
int Fortune::print()
{
    while (true) {
        int val = pick_jar()->print_one(slen, long_only, short_only, show_file);
        if (val > 0) {
            return val;
        }
    }
}

/**
 * Get all strings that match a given regexp pattern.
 */
void Fortune::search(std::string pattern, bool case_insensitive)
{
    int flags = 0;
    if (case_insensitive) {
        flags |= REG_ICASE;
    }

    // Use plain C regex.h, it's way faster than C++ std::regex
    regex_t re;
    regcomp(&re, pattern.c_str(), flags);

    try {
        for (auto cf : jars) {
            cf->print_matches(&re, slen, long_only, short_only);
        }
    } catch (std::exception const& e) {
        regfree(&re);
        throw;
    }

    regfree(&re);
}

void Fortune::print_weights()
{
    for (auto cf : jars) {
        printf(" %6.2f%% %5d %5d %5d  %s\n",
            cf->weight, cf->num_str(), cf->longlen(), cf->shortlen(), cf->path.c_str());
    }
}
