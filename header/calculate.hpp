#include <cmath>
using namespace std;

#define uint unsigned int

uint calculate_high(uint *data, uint data_size, uint l)
{
    uint res = 0, pre = 0;                              // res为最终返回结果、pre为前一个数所在区间
    uint width = (uint)pow(2, l);
    for(int i = 0; i < data_size ; i++)
    {
        uint region = data[i] / width;                 // 求取当前数据所在的区间，width为区间宽度
        res += (region - pre + 1);
        pre = region;
    }
    return (uint)ceil((float)res / 32.0);
}

uint calculate_low(uint data_size, uint l)
{
    return (uint)ceil((float)(data_size * l) / 32.0);
}

uint get_lowbits(uint num, uint l)
{
    uint temp = (1U << l) - 1;
    return temp & num;
}