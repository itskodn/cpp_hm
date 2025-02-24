#include "hm.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>

int main(int argc, char *argv[]){
    int precision =  std::stoi(argv[1]);

    LongNumber pi(0, precision);
    LongNumber n0{1, precision};
    LongNumber denominator(16, precision);
    LongNumber first0{4, precision};
    LongNumber first(1, precision);
    LongNumber second0{2, precision};
    LongNumber second(4, precision);
    LongNumber third0{1, precision};
    LongNumber fourth0{1, precision};

    LongNumber third(5, precision);
    LongNumber fourth(6, precision);
    LongNumber step(8, precision);

    for (int i = 0; i < 4 * precision; ++i)
    {
        pi = pi + n0 * (first0 / first - second0 / second - third0 / third - fourth0 / fourth);
        n0 = n0 / denominator;
        first = first + step;
        second = second + step;
        third = third + step;
        fourth = fourth + step;
    }

    std::cout << pi << '\n';
    return 0;
}

/*
g++ -c hm.cpp -o hm.o
ar cr hm.a hm.o
g++ -c pi.cpp -o pi.o
g++ pi.o hm.a -o pi.exe
*/