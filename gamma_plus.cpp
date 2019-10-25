#include <bitset>
#include <cmath>
#include <fstream>
#include <iostream>
#include <ctime>
using namespace std;

#define uint unsigned int

const int count = 10000;
const int mode_c = 1;

void get_data(uint *data, uint mode);
uint *get_data_piece(uint *data);                                // 读取区间递增的数据
uint calculate_bits(uint num);                                   // 返回一个整数的位数
uint calculate_space(uint *data);                                // 计算编码空间
uint set_high_bits0(uint sub, int num);                          // 数字高num位置0
uint get_high_bits(uint sub, int num);                           // 获取数字高num位
uint get_bits(uint sub, int low, int high);                      // 获取数字从low位到high位的数字
void encode_gamma(uint *gap, uint *encode);// 编码gamma
void encode_gamma_piece(uint *data, uint *encode, uint *infor);  // 编码数据
uint decode_gamma(uint *encode, uint &p, uint &shift);           // 解码gamma
uint decode_data(uint *encode, uint index, uint mode);           // 得到数据
void decode_piece(uint *data, uint *encode, uint *infor);

int main() {
    clock_t startTime,endTime;

    uint *data = new uint[count]();
    // get_data(data, mode_c);
    uint* infor = get_data_piece(data);

    uint sum_space = calculate_space(data);
    uint *encode = new uint[sum_space]();

    float compress_rate = float(sum_space) / float(count);
    cout << "Compress rate:" << compress_rate << endl;

    for(int i = 0; i < 3 ; i++)
    {
        startTime = clock();
        for(int j = 0; j < 10; j++)
        {
            // encode_gamma(data, encode);
            encode_gamma_piece(data, encode, infor);
        }
        endTime = clock();
        cout << "Encode time:" << (endTime - startTime) / 10.0 << endl;
    }

    get_data(data, 2);
    for(int i = 0; i < 3 ; i++)
    {
        startTime = clock();
        for(int j = 0; j < 10; j++)
        {
            // decode_data(encode, count, mode_c);
            decode_piece(data, encode, infor);
        }
        endTime = clock();
        cout << "Decode time:" << (endTime - startTime) / 10.0<< endl;
    }
    

    delete[] data;
    delete[] encode;
    return 0;
}

void get_data(uint *data, uint mode) {
    // data储存原始数据的容器
    // mode表示读取数据的模式,1:求整数之间的gap值;2:直接读取整数;
    ifstream load_data("source\\data.txt");
    uint temp = 0, i = 0;
    switch(mode) {
        case 1: {
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
        case 2: {
            while (load_data >> temp)
            {
                data[i++] = temp;
            }
            break;
        }
        default :
            cout << "invalid mode" << endl;
    }
    load_data.close();
}

uint *get_data_piece(uint *data) {
    uint temp = 0, i = 0;
    ifstream load_data("source\\data.txt");
    while(load_data >> temp) {
        data[i++] = temp;
    }
    load_data.close();

    uint num = 1;                                                // 计算断点的数目，开头的也算，所以num初始值为1
    for(int i = 0; i < count - 1 ; i++) {
        // 统计断点个数
        if(data[i] > data[i + 1]) {
            num++;
        }
    }

    uint *infor = new uint[num * 3 + 1]();
    infor[0] = num;
    infor[1] = 0, infor[2] = 0, infor[3] = 0;
    uint q = 4, pre = data[0], te = pre;
    for (int i = 0; i < count - 1; i++) {
        if(pre > data[i+1]) {
            infor[q++] = i + 1;
            infor[q++] = 0;
            infor[q++] = 0;
            pre = data[i + 1];
        }
        else {
            te = data[i + 1];
            data[i + 1] -= pre;
            pre = te;
        }
    }
    return infor;
}

uint calculate_bits(uint num) {
    // 计算一个数的二进制位数
    return (uint)floor(log(num) / log(2)) + 1;
}

uint calculate_space(uint *data) {
    uint sum_space = 0;
    for (int i = 0; i < count; ++i) {
        sum_space = sum_space + 2 * calculate_bits(data[i]) - 1;
    }
    return (uint) ceil((float) sum_space / 32.0);
}

uint set_high_bits0(uint sub, int num) {
    uint temp = (1U << ((int)floor(log(sub) / log(2)) + 1)) - 1;
    temp = temp >> num;
    return sub & temp;
}

uint get_high_bits(uint sub, int num) {
    uint length_shift = (int)floor(log(sub) / log(2)) + 1 - num;
    return sub >> length_shift;
}

uint get_bits(uint sub, int low, int high) {
    // 获取整数从low位到high位
    uint low_bits = (1U << (low - 1)) - 1;
    uint high_bits = (1U << high) - 1;
    uint temp = high_bits - low_bits;
    sub &= temp;
    return sub >> (low - 1);
}

void encode_gamma(uint *gap, uint *encode) {
    uint p = 0, shift = 32;
    for (int i = 0; i < count; ++i) {
        uint temp = gap[i];
        uint length_redundancy = (int) floor(log(temp) / log(2));   // 编码冗余位的长度，即gamma编码前0串的长度
        uint length_code = (int) floor(log(temp) / log(2)) * 2 + 1; // gamma编码长度

        if (shift == 0) {
            // 如果剩余空位为0，则指针后移重置shift
            p++;
            shift = 32;
        }
        // 判断两种种情况，剩余空位数与编码长度的比较
        if (shift >= length_code) {
            // 如果剩余空位大于编码长度，去除冗余编码长度后，将temp左移shift-length_redundancy-1位后与encode[p]按位或
            shift -= length_redundancy;
            encode[p] |= temp << (shift - length_redundancy - 1);
            shift = shift - length_redundancy - 1;
        }
        else {
            // 如果剩余空位小于编码长度，较为复杂分为以下几种情况讨论
            if (shift == length_redundancy) {
                // 如果冗余编码长度与剩余空位相同，则重置shift，并将p指针后移
                shift = 32 - (int) floor(log(temp) / log(2)) - 1;
                encode[++p] |= temp << shift;
            }
            else if (shift > length_redundancy) {
                // 如果剩余空位大于冗余编码长度，则减去冗余编码长度后，取temp高shift位，并将temp高shift置零，重置shift
                shift -= length_redundancy;
                int high_bits = get_high_bits(temp, shift);
                encode[p] |= high_bits;
                high_bits = set_high_bits0(temp, shift);
                p++;
                shift = 32 - (length_redundancy + 1 - shift);
                encode[p] |= temp << shift;
            }
            else {
                // 如果剩余空位小于冗余编码长度，则冗余编码长度减去shift后，shift = 32 - k，表示剩余冗余编码长度，指针后移
                int k = length_redundancy - shift;
                p++;
                shift = 32 - k;
                if (shift >= length_redundancy + 1) {
                    // 如果剩余空位比temp二进制格式长，直接左移后添加到encode中
                    shift = shift - length_redundancy - 1;
                    encode[p] |= temp << shift;
                }
                else {
                    // 如果剩余空位比temp二进制格式短。取高shift位，指针后移，temp高shift位置零
                    int high_bits = get_high_bits(temp, shift);
                    encode[p++] |= high_bits;
                    temp = set_high_bits0(temp, shift);
                    shift = 32 - (length_redundancy + 1 - shift);
                    encode[p] |= temp << shift;
                }
            }
        }
    }
}

void encode_gamma_piece(uint *data, uint *encode, uint *infor){
    uint p = 0, shift = 32, q = 1;
    for(int i = 0; i < count ; i++) {
        if(infor[q] == i) {
            // 即当前数据处于断点位置，记录编码指针和整数内部偏移
            infor[++q] = p;
            infor[++q] = shift;
            ++q;
        }

        uint temp = data[i];
        uint length_redundancy = (int) floor(log(temp) / log(2));   // 编码冗余位的长度，即gamma编码前0串的长度
        uint length_code = (int) floor(log(temp) / log(2)) * 2 + 1; // gamma编码长度

        if (shift == 0) {
            // 如果剩余空位为0，则指针后移重置shift
            p++;
            shift = 32;
        }
        // 判断两种种情况，剩余空位数与编码长度的比较
        if (shift >= length_code) {
            // 如果剩余空位大于编码长度，去除冗余编码长度后，将temp左移shift-length_redundancy-1位后与encode[p]按位或
            shift -= length_redundancy;
            encode[p] |= temp << (shift - length_redundancy - 1);
            shift = shift - length_redundancy - 1;
        }
        else {
            // 如果剩余空位小于编码长度，较为复杂分为以下几种情况讨论
            if (shift == length_redundancy) {
                // 如果冗余编码长度与剩余空位相同，则重置shift，并将p指针后移
                shift = 32 - (int) floor(log(temp) / log(2)) - 1;
                encode[++p] |= temp << shift;
            }
            else if (shift > length_redundancy) {
                // 如果剩余空位大于冗余编码长度，则减去冗余编码长度后，取temp高shift位，并将temp高shift置零，重置shift
                shift -= length_redundancy;
                int high_bits = get_high_bits(temp, shift);
                encode[p] |= high_bits;
                high_bits = set_high_bits0(temp, shift);
                p++;
                shift = 32 - (length_redundancy + 1 - shift);
                encode[p] |= temp << shift;
            }
            else {
                // 如果剩余空位小于冗余编码长度，则冗余编码长度减去shift后，shift = 32 - k，表示剩余冗余编码长度，指针后移
                int k = length_redundancy - shift;
                p++;
                shift = 32 - k;
                if (shift >= length_redundancy + 1) {
                    // 如果剩余空位比temp二进制格式长，直接左移后添加到encode中
                    shift = shift - length_redundancy - 1;
                    encode[p] |= temp << shift;
                }
                else {
                    // 如果剩余空位比temp二进制格式短。取高shift位，指针后移，temp高shift位置零
                    int high_bits = get_high_bits(temp, shift);
                    encode[p++] |= high_bits;
                    temp = set_high_bits0(temp, shift);
                    shift = 32 - (length_redundancy + 1 - shift);
                    encode[p] |= temp << shift;
                }
            }
        }
        // cout << bitset<32>(encode[p]) << " " << p << " " << shift << "\n";
    }
}

uint decode_gamma(uint *encode, uint &p, uint &shift) {
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

uint decode_data(uint *encode, uint index, uint mode) {
    // encode表示编码序列
    // index表示需要解码的数所在位置
    // mode表示数据对应关系，1:严格递增数列;2:随机数列;3:区间递增数列;
    uint p = 0, shift = 32, res = 0;
    switch(mode) {
        case 1: {
            for(int i = 0; i < index ; i++)
                res += decode_gamma(encode, p, shift);
            break;
        }   
        case 2: {
            for(int i = 0; i < index - 1 ; i++)
                decode_gamma(encode, p, shift);
            // cout << p << " " << shift << "\n";
            res = decode_gamma(encode, p, shift);
            break;
        }
        default :
            cout << "invalid mode!" << endl;          
    }
    return res;
}

void decode_piece(uint *data, uint *encode, uint *infor)
{
    uint inf = 0;                        // 信息数组的指针
    uint res = 0;
    uint p = 0, shift = 32;
    uint pre = 0;
    uint cnt = 0;
    for(int i = 0; i < count; i++)
    {
        if(i==infor[inf * 3 + 1])
        {
            res = decode_gamma(encode, p, shift);
            inf++;
        }
        else
            res = pre + decode_gamma(encode, p, shift);

        pre = res;
        if(res == data[i])
            cnt++;
        else{
            cout << "There is something wrong with " << i << endl;
            break;
        }
    }
}

