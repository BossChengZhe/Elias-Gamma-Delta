#include <cmath>
#include <iostream>
#include <bitset>
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
        cout << bitset<32>(data[i]) << " " << data[i] << endl;
        if(shift == 0)
        {
            p++;
            shift = 32;
        }

        uint temp = get_lowbits(data[i], l);

        if(shift >= l){
            // 如果剩余位数大于l
            shift -= l;
            low_bits[p] |= temp << shift;
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

uint get_num_array(uint *array, uint l, uint index)
{
    // index是从1开始的无符号整形
    uint temp = l * (index - 1);
    uint shift = 32 - (temp % 32), p = temp / 32;
    uint res = 0;
    for (int i = 0; i < l; i++)
    {
        if(shift == 0)
        {
            p++;
            shift = 32;
        }
        res <<= 1;
        uint bit = array[p] & (1U << (shift - 1));
        res |= bit >> (shift - 1);
        shift--;
    }
    return res;
}

uint count_0(uint *high_bits, uint &p, uint &shift)
{
    uint res = 0;
    while(1)
    {
        if(shift == 0)
        {
            p++;
            shift = 32;
        }
        shift--;
        uint flag = high_bits[p] & (1U << shift);
        if(flag == 0)
            res++;
        else
            break;
    }
    return res;
}

uint decode_fano(uint *data, uint *high_bits, uint *low_bits, uint data_size, uint l)
{
    uint shift = 32, p = 0;
    uint count = 0, pre = 0;                                             // 记录解码正确的个数
    uint width = (uint)pow(2, l);
    uint res = 0;
    for(int i = 0; i < data_size ; i++)
    {
        if(shift == 32)
        {
            p++;
            shift = 32;
        }
        uint num_0 = count_0(high_bits, p, shift);
        pre += num_0;
        res = pre * width + get_num_array(low_bits, l, i + 1);
        if(data[i] == res)
            count++;
        else
        {
            cout << i << endl;
            break;
        }
    }
    return count;
}