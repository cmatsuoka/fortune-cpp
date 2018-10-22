#include <iostream>
#include <cstdlib>
#include <ctime>
#include "fortune.h"

int main(int argc, char **argv)
{
    srand((int)time(0));
    Fortune fortune{};

    try {
        fortune.load("/usr/share/games/fortunes/wisdom", 1.0);
        fortune.print();
    } catch (std::exception& e) {
        std::cerr << argv[0] << ": " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}
