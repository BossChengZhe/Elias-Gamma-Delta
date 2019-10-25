#include <iostream>
#include <fstream>
#include <bitset>
#include "header/calculate.hpp"
#include "header/get_data.hpp"
#include "header/Fano.hpp"
#include <ctime>
using namespace std;

#define uint unsigned int

const uint count = 1000000;

int main()
{
    clock_t startTime,endTime;
    
    uint *data = new uint[count]();
    get_data(data);
    uint l = calculate_l(data, count);
    uint space_low = calculate_low(count, l);
    uint space_high = calculate_high(data, count, l);

    float compress_rate = float(space_high + space_low) / (float)count;
    cout << "Compress rate:" << compress_rate << endl;

    uint *high_bits = new uint[space_high]();
    uint *low_bits = new uint[space_low]();
    for(int i = 0; i < 3 ; i++)
    {
        startTime = clock();
        store_low_bits(data, low_bits, count, l);
        store_high_bits(data, high_bits, count, l);
        endTime = clock();
        cout << "Encode time:" << endTime - startTime << endl;
    }

    for(int i = 0; i < 3 ; i++)
    {
        startTime = clock();
        decode_fano(data, high_bits, low_bits, count, l);
        endTime = clock();
        cout << "decode time:" << endTime - startTime << endl;
    }


    return 0;
}