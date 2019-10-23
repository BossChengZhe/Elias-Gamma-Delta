#include <iostream>
#include <fstream>
#include <bitset>
#include "header/calculate.hpp"
#include "header/get_data.hpp"
#include "header/Fano.hpp"
using namespace std;

#define uint unsigned int

const uint count = 1000000;

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

    ofstream low("source\\low_bits.txt", ios_base::trunc);
    ofstream high("source\\high_bits.txt", ios_base::trunc);
    for(int i = 0; i < space_low; i++)
        low << bitset<32>(low_bits[i]) << endl;
    for(int i = 0; i < space_high; i++)
        high << bitset<32>(high_bits[i]) << endl;
    low.close();
    high.close();

    uint res = decode_fano(data, high_bits, low_bits, count, l);
    cout << res << endl;

    return 0;
}