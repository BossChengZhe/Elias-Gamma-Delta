#include <iostream>
#include <fstream>
#include "header/calculate.hpp"
#include "header/get_data.hpp"
#include "header/Fano.hpp"

using namespace std;

#define uint unsigned int

const uint count = 1000;

int main()
{
    uint *data = new uint[count]();
    get_data(data);
    return 0;
}