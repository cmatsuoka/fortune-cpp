#include <iostream>
#include <cstdlib>
#include <ctime>
#include <sys/time.h>
#include "fortune.h"


int main(int argc, char **argv)
{
    struct timeval t;
    gettimeofday(&t, NULL);

    srand(t.tv_sec * 1000 + t.tv_usec / 1000);
    Fortune fortune{};

    try {
        fortune.load("/usr/share/games/fortunes", 1.0);
        fortune.print();
    } catch (std::exception& e) {
        std::cerr << argv[0] << ": " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}
