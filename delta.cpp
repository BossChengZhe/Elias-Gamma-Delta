#include <bitset>
#include <cmath>
#include <fstream>
#include <iostream>
using namespace std;

#define uint unsigned int

const int count = 1000; // count为数据规模

void get_data(uint *data, uint mode);                            // 获取完整数据
uint calculate_bits(uint num);                                   // 返回一个整数的位数
uint calculate_space(uint *data);                                // 计算编码空间
uint set_high_bits0(uint sub, int num);                          // 数字高num位置0
uint get_high_bits(uint sub, int num);                           // 获取数字高num位
uint get_bits(uint sub, int low, int high);                      // 获取数字从low位到high位的数字
void encode_gamma(uint num, uint *encode, uint &p, uint &shift); // 对数字num进行gamma编码
void encode_delta(uint *data, uint *encode);                     // 对于数据进行delta编码
uint decode_gamma(uint *encode, uint &p, uint &shift);           // 解gamma编码
uint decode_delta(uint *encode, uint &p, uint &shift);           // 解delta编码
uint decode_data(uint *encode, uint index, uint mode);           // 由于分为好几种形式，gap、直接编码，和区间递增，故采用单独的函数获得数据

int main()
{
    // uint data_mode, decode_mode;
    // cin >> data_mode >> decode_mode;

    uint *data = new uint[count]();
    get_data(data, 1);
    uint sum_space = calculate_space(data);
    uint *encode = new uint[sum_space]();
    encode_delta(data, encode);

    ofstream test("result.txt", ios_base::trunc);
    for(int i = 0; i < sum_space ; i++)
    {
        test << bitset<32>(encode[i]) << endl;
    }
    test.close();

    get_data(data, 1);
    uint num = 0;
    for(int i = 0; i < count ; i++)
    {
        uint res = decode_data(encode, i + 1, 2);
        if(res == data[i])
            num++;
        else { 
            cout << i << endl;
            break;
        }
    }
    cout << num << endl;

    delete[] data;
    delete[] encode;
    return 0;
}

void get_data(uint *data, uint mode)
{
    // data储存原始数据的容器
    // mode表示读取数据的模式,1:求整数之间的gap值;2:直接读取整数;
    ifstream load_data("data.txt");
    uint temp = 0, i = 0;
    switch(mode) {
        case 1: {
            while (load_data >> temp)
            {
                data[i++] = temp;
            }
            break;
        }  
        case 2: {
            i++;
            uint pre = 0;
            load_data >> temp;
            data[0] = temp;
            pre = temp;
            while (load_data >> temp)
            {
                data[i++] = temp - pre;
                pre = temp;
            }
            break;
        }
        default :
            cout << "invalid mode" << endl;
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
            shift = 32 - (length_num - 1 - shift) - length_num;
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
        // cout << data[i] << '\n';
        encode_gamma(calculate_bits(data[i]), encode, p, shift);
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
        // cout << bitset<32>(encode[p]) << " " << p << " " << shift << '\n';
    }
}

uint decode_gamma(uint *encode, uint &p, uint &shift)
{
    uint num = 0;
    while(1) {
        // 判断连续0的个数
        if(shift == 0) {
            p++;
            shift = 32;
        }
        uint flag = encode[p] & (1U << (shift - 1));
        shift--;
        if(flag != 0) {
            shift++;
            break;
        }
        else
            num++;
    }
    uint temp = 0;
    for(int j = 0; j < num + 1 ; j++) {
        // 解码有效部分
        if(shift == 0) {
            p++;
            shift = 32;
        }
        temp = temp << 1;
        uint flag = encode[p] & (1U << (shift - 1));
        shift--;
        temp |= flag >> shift;
    }
    return temp;
}

uint decode_delta(uint *encode, uint &p, uint &shift)
{
    uint res = 1;
    uint num_bits = decode_gamma(encode, p, shift);
    for(int i = 0; i < num_bits - 1; i++)
    {
        if(shift == 0) {
            p++;
            shift = 32;
        }
        res <<= 1;
        uint temp = encode[p] & (1U << (shift - 1));
        res |= temp >> (shift - 1);
        shift--;
    }
    return res;
}              

uint decode_data(uint *encode, uint index, uint mode)
{
    // encode表示编码序列
    // index表示需要解码的数所在位置
    // mode表示数据对应关系，1:严格递增数列;2:随机数列;3:区间递增数列;
    uint p = 0, shift = 32, res = 0;
    switch(mode) {
        case 1:
            for(int i = 0; i < index ; i++)
                res += decode_delta(encode, p, shift);
            break;
        case 2:
            for(int i = 0; i < index - 1 ; i++)
            {
                // 按照编码流程走一遍，但不出编码结果，只是确定位
                uint temp = decode_gamma(encode, p, shift);
                if(temp - 1 > shift) {
                    //去掉高位后的数字位数大于剩余空位shift
                    p++;
                    shift = 32 - (temp - 1 - shift);
                }
                else if(temp - 1 == shift) {
                    //去掉高位后的数字位数等于剩余空位shift
                    p++;
                    shift = 32;
                }
                else
                    shift -= (temp - 1);
            }
            res = decode_delta(encode, p, shift);
            break;
        case 3:
            cout << "There is no such mode!" << endl;
            break;
        default :
            cout << "invalid mode!" << endl;
    }
    return res;
}