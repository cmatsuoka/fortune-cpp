#include <sys/time.h>
#include <unistd.h>
#include <map>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "fortune.h"

#define MIN_WAIT      6
#define CHARS_PER_SEC 20
#define FORTUNE_DIR   "/usr/share/games/fortunes"

void usage(char *cmd);


int main(int argc, char **argv)
{
    struct timeval t;
    gettimeofday(&t, NULL);
    srand(t.tv_sec * 1000 + t.tv_usec / 1000);

    Fortune fortune{};

    auto equal_size = false;

    int c;
    while ((c = getopt(argc, argv, "acefhilm:n:osw")) != -1) {
        switch (c) {
        case 'a':
            fortune = fortune.all();
            break;
        case 'o':
            fortune = fortune.offensive();
            break;
        case 'e':
            equal_size = true;
            break;
        case 'l':
            fortune = fortune.long_fortunes();
            break;
        case 's':
            fortune = fortune.short_fortunes();
            break;
        case 'c':
            fortune = fortune.show_filename();
            break;
        case 'n':
            fortune = fortune.short_len(std::stoi(std::string(optarg)));
            break;
        case 'h':
            usage(argv[0]);
            exit(EXIT_SUCCESS);
        }
    }

    std::map<std::string, float> filemap{};
    if (optind == argc) {
        filemap[FORTUNE_DIR] = -1.0;
    } else {
        for (int i = optind; i < argc; i++) {
            filemap[argv[i]] = 1.0;
        }
    }

    try {
        for (auto it = filemap.begin(); it != filemap.end(); it++) {
            auto key = it->first;
            fortune.load(key, filemap[key]);
        }
        fortune.print();
    } catch (std::exception& e) {
        std::cerr << argv[0] << ": " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void usage(char *cmd)
{
    std::cout << ""
"Usage: " << cmd << " [options] [[n%] file/dir]...\n"
"Options:\n"
"    -a          choose from all lists of maxims\n"
"    -c          show the cookie file from which the fortune came\n"
"    -e          consider all fortune files to be of equal size\n"
"    -f          print out the list of files to be searched\n"
"    -h          display usage information and exit\n"
"    -i          ignore case for -m patterns\n"
"    -l          long dictums only\n"
"    -m pattern  print all fortunes matching the regex\n"
"    -n len      set the longest length considered short\n"
"    -o          choose only from potentially offensive aphorisms\n"
"    -s          short apothegms only\n"
"    -w          wait before termination"
    << std::endl;
}
