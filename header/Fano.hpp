#include <cmath>
using namespace std;

#define uint unsigned int

uint get_lowbits(uint num, uint l)
{
    uint temp = (1U << l) - 1;
    return temp & num;
}

uint get_bits(uint sub, int low, int high) {
    // 获取整数从low位到high位
    uint low_bits = (1U << (low - 1)) - 1;
    uint high_bits = (1U << high) - 1;
    uint temp = high_bits - low_bits;
    sub &= temp;
    return sub >> (low - 1);
}

void store_low_bits(uint *data, uint *low_bits, uint data_size, uint l)
{
    uint shift = 32, p = 0;
    for(int i = 0; i < data_size ; i++)
    {
        if(shift == 0)
        {
            p++;
            shift = 32;
        }

        uint temp = get_lowbits(data[i], l);

        if(shift >= l){
            // 如果剩余位数大于l
            shift -= l;
            low_bits[i] |= temp << shift;
        }
        else{
            // 如果剩余位数不够存储
            uint temp1 = get_bits(temp, shift + 1, l);
            low_bits[p++] |= temp1;
            shift = 32 - (l - shift);
            temp = get_lowbits(temp, shift);
            low_bits[p] |= temp;
        }
    }
}

void store_high_bits(uint *data, uint *high_bits, uint data_size, uint l)
{
    uint width = (uint)pow(2, l), pre = 0;
    uint shift = 32, p = 0;
    for(int i = 0; i < data_size ; i++)
    {
        if(shift == 32)
        {
            shift = 32;
            p++;
        }
        uint region = data[i] / width;
        if(shift > region - pre)
            shift -= (region - pre);
        else{
            shift = 32 - (region - pre - shift);
            p++;
        }
        high_bits[p] |= 1U << (shift - 1);
        shift--;
        pre = region;
    }
}

uint decode_fano(uint *data, uint *high_bits, uint *low_bits, uint data_size, uint l)
{
    uint shift = 32, p = 0;
    uint sum = 0;
    for(int i = 0; i < data_size ; i++)
    {
        if(shift == 32)
        {
            p++;
            shift = 32;
        }
        
    }
}