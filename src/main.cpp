#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <map>
#include <iostream>
#include <algorithm>
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

    Fortune fortune(FORTUNE_DIR);

    auto equal_size = false;
    auto list_files = false;
    auto case_insensitive = false;
    auto wait_fortune = false;
    auto pattern = std::string();

    int c;
    while ((c = getopt(argc, argv, "acefhilm:n:osw")) != -1) {
        switch (c) {
        case 'a':  // list all fortunes
            fortune.all();
            break;
        case 'c':  // show file name
            fortune.show_filename();
            break;
        case 'e':  // assume all files have equal size
            equal_size = true;
            break;
        case 'f':  // list file probabilities
            list_files = true;
            break;
        case 'h':  // show usage information and exit
            usage(argv[0]);
            exit(EXIT_SUCCESS);
        case 'i':  // case insensitive matches
            case_insensitive = true;
            break;
        case 'l':  // show long messages only
            fortune.long_fortunes();
            break;
        case 'm':  // search regexp matches
            pattern = std::string(optarg);
            break;
        case 'n':  // set the short message threshold
            fortune.short_len(std::stoi(std::string(optarg)));
            break;
        case 'o':  // choose from offensive messages
            fortune.offensive();
            break;
        case 's':  // show short messages only
            fortune.short_fortunes();
            break;
        case 'w':  // wait before termination
            wait_fortune = true;
            break;
        default:
            exit(EXIT_FAILURE);
        }
    }

    // Get file list from command line with optional percentages
    std::map<std::string, float> filemap{};
    if (optind == argc) {
        filemap[FORTUNE_DIR] = -1.0;
    } else {
        float percentage = -1.0f;
        for (int i = optind; i < argc; i++) {
            char *arg = argv[i];
            if (*arg != '\0' && arg[strlen(arg) - 1] == '%') {
                percentage = strtof(arg, NULL);
            } else {
                filemap[arg] = percentage;
                percentage = -1.0f;
            }
        }
    }

    try {
        // load metadata
        for (auto it = filemap.begin(); it != filemap.end(); it++) {
            auto key = it->first;
            fortune.load(key, filemap[key]);
        }

        if (equal_size) {
            fortune.equal_size();
        }

        fortune.normalize_weights();

        if (list_files) {
            fortune.print_weights();
            exit(EXIT_SUCCESS);
        }

        if (pattern != "") {
            fortune.search(pattern, case_insensitive);
        } else {
            int fort_size = fortune.print();
            if (wait_fortune) {
                sleep(std::max(fort_size / CHARS_PER_SEC, MIN_WAIT));
            }
        }
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
