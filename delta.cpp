#include <bitset>
#include <cmath>
#include <fstream>
#include <iostream>
using namespace std;

#define uint unsigned int

const int count = 1000; // count为数据规模

void get_gap(uint *gap);                                         // 获取数据后项减前项的和
void get_data(uint *data);                                       // 获取完整数据
uint calculate_bits(uint num);                                   // 返回一个整数的位数
uint calculate_space(uint *data);                                // 计算编码空间
uint set_high_bits0(uint sub, int num);                          // 数字高num位置0
uint get_high_bits(uint sub, int num);                           // 获取数字高num位
uint get_bits(uint sub, int low, int high);                      // 获取数字从low位到high位的数字
void encode_gamma(uint num, uint *encode, uint &p, uint &shift); // 对数字num进行gamma编码
void encode_delta(uint *data, uint *encode);                     // 对于数据进行delta编码

int main()
{
    uint x = 0;
    cin >> x;
    cout << bitset<32>(x) << endl;
    x = get_bits(x, 3, 7);
    cout << bitset<32>(x) << endl;
    return 0;
}

void get_gap(uint *gap)
{
    ifstream load_data("F:\\gamma\\data.txt");
    int temp = 0, pre = 0, i = 1;
    load_data >> temp;
    gap[0] = temp;
    pre = temp;
    while (load_data >> temp)
    {
        gap[i++] = temp - pre;
        pre = temp;
    }
    load_data.close();
}

void get_data(uint *data)
{
    ifstream load_data("F:\\gamma\\data.txt");
    uint temp = 0, i = 0;
    while (load_data >> temp)
    {
        data[i++] = temp;
    }
    load_data.close();
}

uint calculate_bits(uint num)
{
    return (uint)floor(log(num) / log(2)) + 1;
}

uint calculate_space(uint *data) {
    // detal编码由两部分构成，对数据的位数进行gamma编码，和去除数据最高位
    // 第一部分编码长度: 2*calculate_bits(calculate_bits(num)) - 1
    // 第二部分编码长度: calculate_bits(num) - 1
    uint res = 0;
    for(int i = 0; i < count ; i++)
    {
        res = res + (2 * calculate_bits(calculate_bits(data[i])) + calculate_bits(data[i]) - 2);
    }
    return (uint) ceil((float) res / 32.0);
}

uint set_high_bits0(uint sub, int num)
{
    uint temp = (1U << ((int)floor(log(sub) / log(2)) + 1)) - 1;
    temp = temp >> num;
    return sub & temp;
}

uint get_high_bits(uint sub, int num)
{
    uint length_shift = (int)floor(log(sub) / log(2)) + 1 - num;
    return sub >> length_shift;
}

uint get_bits(uint sub, int low, int high)
{
    uint low_bits = (1U << (low - 1)) - 1;
    uint high_bits = (1U << high) - 1;
    uint temp = high_bits - low_bits;
    sub &= temp;
    return sub >> (low - 1);
}

void encode_gamma(uint num, uint *encode, uint& p, uint& shift) {
    // num为gamma编码的目标数字，encode为存储编码序列的数组，
    // p为序列指针,shift为uint内剩余未存储编码数据的位
    uint length_num = calculate_bits(num);
    if(shift == 0) {
        p++;
        shift = 32;
    }
    if(shift >= 2*length_num - 1) {
        // 如果剩余空位大于编码长度，去除冗余编码长度后，将temp左移shift-length_redundancy-1位后与encode[p]按位或
        shift -= (length_num - 1);
        encode[p] |= num << (shift - length_num);
        shift -= length_num;
    }
    else {
        if(shift == length_num - 1) {
            // 如果冗余编码长度与剩余空位相同，则重置shift，并将p指针后移
            shift = 32 - length_num;
            encode[++p] |= num << shift;
        }
        else if(shift > length_num - 1) {
            // 如果剩余空位大于冗余编码长度，则减去冗余编码长度后，取temp高shift位，并将temp高shift置零，重置shift
            shift -= (length_num - 1);
            uint high_bits = get_high_bits(num, shift);
            encode[p++] |= high_bits;
            shift = 32 - (length_num - shift);
            encode[p] |= num << shift;
        }
        else {
            // 如果剩余空位小于冗余编码长度，则冗余编码长度减去shift后，shift = 32 - k，表示剩余冗余编码长度，指针后移
            p++;
            shift = 32 - 2 * length_num - 1 + shift;
            encode[p] |= num << shift;
        }
    }
}

void encode_delta(uint *data, uint *encode) {
    uint p = 0, shift = 32;
    for(int i = 0; i < count ; i++)
    {
        if(shift == 0) {
            p++;
            shift = 32;
        }
        encode_gamma(data[i], encode, p, shift);
        uint length_data = calculate_bits(data[i]) - 1;
        uint temp = set_high_bits0(data[i], 1);                // 去掉数的最高位，即最高位置零，数字长度减一
        
        if(shift >= length_data) {
            shift -= length_data;
            encode[p] |= temp << shift;
        }
        else {
            uint bits = get_bits(temp, length_data - shift + 1, length_data);
            encode[p++] |= bits;
            bits = get_bits(temp, 1, length_data - shift);
            length_data -= shift;
            shift = 32;
            shift -= length_data;
            encode[p] |= bits << shift;
        }
    }
}