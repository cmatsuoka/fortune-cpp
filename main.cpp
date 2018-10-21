#include <cstdlib>
#include <ctime>
#include "fortune.h"

int main(int argc, char **argv)
{
    srand((int)time(0));
    Fortune *fortune = new Fortune();
    fortune->load("/usr/share/games/fortune/wisdom", 1.0);
    fortune->print();
}
