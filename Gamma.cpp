#include <iostream>
#include <fstream>
#include <cmath>
#include <bitset>
using namespace std;

const int count = 60000;                      // 数据规模

void get_data(int *gap);                     // get data from text file
int calculate_space(int *gap);               // calculate the space of 01 stream after encode
int get_high_bits(int sub, int num);                  // 取数字的高num位
int get_low_bits(int sub, int num);                   // 取数字低num位
int set_high_bits0(int sub, int num);                 // 数字高num位置0
void encode_gamma(int *gap, unsigned int *encode);    // gamma编码，并存储到encode数组中
unsigned int decode_gamma(unsigned int *encode, int subscript);    // 解gamma编码

int main() {
    ifstream data("C:\\Homework\\Code\\data.txt");
    int *original_data = new int[count]();
    int mid=0, lo = 0;
    while(data >> mid) {
        original_data[lo++] = mid;
    }
    data.close();

    int *gap = new int[count]();
    get_data(gap);
    int sum_space = calculate_space(gap);
    unsigned int *encode = new unsigned int[sum_space]();
    encode_gamma(gap, encode);

    int flag = 0;
    for(int i = 0; i < count ; i++)
    {
        if(original_data[i] == decode_gamma(encode, i+1))
            flag++;
        else
            cout << "there is something wrong:" << i << endl;
    }
    cout << flag << endl;
    delete[] gap;
    delete[] encode;
    delete[] original_data;
    return 0;
}

void get_data(int *gap) {
    ifstream test;
    test.open("C:\\Homework\\Code\\data.txt");
    int temp = 0, pre = 0, i = 1;
    test >> temp;
    gap[0] = temp;
    pre = temp;
    while (test >> temp) {
        gap[i++] = temp - pre;
        pre = temp;
    }
    test.close();
}

int calculate_space(int *gap) {
    int sum_space = 0;
    for (int i = 0; i < count; ++i) {
        int temp = (int) floor(log(gap[i]) / log(2)) * 2 + 1;
        sum_space += temp;
    }
    return (int) ceil((float) sum_space / 32.0);
}

int get_high_bits(int sub, int num) {
    unsigned int temp = (1U << ((int)floor(log(sub) / log(2)) + 1)) - 1;
    int length_temp = (int) floor(log(temp) / log(2)) + 1 - num;
    if(length_temp < 0)
        // 比方说一共有4位，要求取最高的5位，返回-1，表示超限
        return -1;
    temp = temp >> length_temp;
    temp = temp << length_temp;
    sub &= temp;
    sub = sub >> length_temp;
    return sub;
}

int get_low_bits(int sub, int num) {
    unsigned int temp = (1U << num) - 1;
    return sub & temp;
}

int set_high_bits0(int sub, int num) {
    unsigned int temp = (1U << ((int)floor(log(sub) / log(2)) + 1)) - 1;
    temp = temp >> num;
    return sub & temp;
}

void encode_gamma(int *gap, unsigned int *encode) {
    int left = 32;                  // 一个整数的剩余空位，既没有存储编码的地方
    int p = 0;                      // 编码指针
    for (int i = 0; i < count; ++i) {
        unsigned int temp = gap[i];
        int length_redundancy = (int) floor(log(temp) / log(2));   // 编码冗余位的长度，即gamma编码前0串的长度
        int length_code = (int) floor(log(temp) / log(2)) * 2 + 1; // gamma编码长度

        if (left == 0) {
            // 如果剩余空位为0，则指针后移重置left
            p++;
            left = 32;
        }
        // 判断两种种情况，剩余空位数与编码长度的比较
        if (left >= length_code) {
            // 如果剩余空位大于编码长度，去除冗余编码长度后，将temp左移left-length_redundancy-1位后与encode[p]按位或
            left -= length_redundancy;
            encode[p] |= temp << (left - length_redundancy - 1);
            left = left - length_redundancy - 1;
        }
        else {
            // 如果剩余空位小于编码长度，较为复杂分为以下几种情况讨论
            if (left == length_redundancy) {
                // 如果冗余编码长度与剩余空位相同，则重置left，并将p指针后移
                left = 32 - (int) floor(log(temp) / log(2)) - 1;
                encode[++p] |= temp << left;
            }
            else if (left > length_redundancy) {
                // 如果剩余空位大于冗余编码长度，则减去冗余编码长度后，取temp高left位，并将temp高left置零，重置left
                left -= length_redundancy;
                int high_bits = get_high_bits(temp, left);
                encode[p] |= high_bits;
                high_bits = set_high_bits0(temp, left);
                p++;
                left = 32 - (length_redundancy + 1 - left);
                encode[p] |= temp << left;
            }
            else {
                // 如果剩余空位小于冗余编码长度，则冗余编码长度减去left后，left = 32 - k，表示剩余冗余编码长度，指针后移
                int k = length_redundancy - left;
                p++;
                left = 32 - k;
                if (left >= length_redundancy + 1) {
                    // 如果剩余空位比temp二进制格式长，直接左移后添加到encode中
                    left = left - length_redundancy - 1;
                    encode[p] |= temp << left;
                }
                else {
                    // 如果剩余空位比temp二进制格式短。取高left位，指针后移，temp高left位置零
                    int high_bits = get_high_bits(temp, left);
                    encode[p++] |= high_bits;
                    temp = set_high_bits0(temp, left);
                    left = 32 - (int) floor(log(temp) / log(2)) - 1;
                    encode[p] |= temp << left;
                }
            }
        }
    }
}

unsigned int decode_gamma(unsigned int *encode, int subscript) {
    int p = 0, bit = 31;             // p为encode指针，bit为encode整数内部偏移量
    unsigned int res = 0;
    for (int i = 0; i < subscript; ++i) {
        int num = 0;                // num为0的个数
        while(1) {
            // 判断连续零的个数
            if(bit < 0) {
                p++;
                bit = 31;
            }
            unsigned int flag = encode[p] & (1U << bit);
            bit--;
            if(flag != 0)
            {
                bit++;
                break;
            }
            else
                num++;
        }
        unsigned int temp = 0;
        for(int j = 0; j < num + 1 ; j++){
            if(bit < 0) {
                p++;
                bit = 31;
            }
            temp = temp << 1;
            unsigned int flag = encode[p] & (1U << bit);
            flag = flag >> bit;
            bit--;
            temp |= flag;
        }
        res += temp;
    }
    return res;
}