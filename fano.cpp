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
    uint l = calculate_l(data, count);
    uint space_low = calculate_low(count, l);
    uint space_high = calculate_high(data, count, l);
    uint *high_bits = new uint[space_high]();
    uint *low_bits = new uint[space_low]();
    store_low_bits(data, low_bits, count, l);
    store_high_bits(data, high_bits, count, l);
    return 0;
}