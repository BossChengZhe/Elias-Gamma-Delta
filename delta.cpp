#include <bitset>
#include <cmath>
#include <fstream>
#include <iostream>
#include <ctime>
using namespace std;

#define uint unsigned int

const int count = 100000; // count为数据规模
const int mode_c = 1;

void get_data(uint *data, uint mode);                            // 获取完整数据
uint *get_data_piece(uint *data);
uint calculate_bits(uint num);                                   // 返回一个整数的位数
uint calculate_space(uint *data);                                // 计算编码空间
uint set_high_bits0(uint sub, int num);                          // 数字高num位置0
uint get_high_bits(uint sub, int num);                           // 获取数字高num位
uint get_bits(uint sub, int low, int high);                      // 获取数字从low位到high位的数字
void encode_gamma(uint num, uint *encode, uint &p, uint &shift); // 对数字num进行gamma编码
void encode_delta(uint *data, uint *encode);                     // 对于数据进行delta编码
void encode_delta_piece(uint *data, uint *encode, uint *infor);
uint decode_gamma(uint *encode, uint &p, uint &shift);           // 解gamma编码
uint decode_delta(uint *encode, uint &p, uint &shift);           // 解delta编码
void decode_data(uint *data, uint *encode, uint mode);           // 由于分为好几种形式，gap、直接编码，和区间递增，故采用单独的函数获得数据
void decode_piece(uint *data, uint *encode, uint *infor, uint data_size);

int main()
{
    clock_t startTime,endTime;

    uint *data = new uint[count]();

    // get_data(data, mode_c);
    uint *infor = get_data_piece(data);

    uint sum_space = calculate_space(data);
    uint *encode = new uint[sum_space]();

    float compress_rate = float(sum_space) / float(count);
    cout << "Compress rate:" << compress_rate << endl;

    for(int i = 0; i < 3 ; i++)
    {
        startTime = clock();
        
        for(int j = 0; j < 10; j++)
        {
            // encode_delta(data, encode);
            encode_delta_piece(data, encode, infor);
        }
        
        endTime = clock();
        cout << "Encode time:" << (endTime - startTime) / 10.0 << endl;
    }

    ofstream re("source\\result.txt", ios_base::trunc);
    for(int i = 0; i < sum_space ; i++)
    {
        re << bitset<32>(encode[i]) << endl;
    }
    re.close();

    get_data(data, 2);
    for(int i = 0; i < 3 ; i++)
    {
        startTime = clock();
        for(int j = 0; j < 10; j++)
        {
            // decode_data(data, encode, mode_c);
            decode_piece(data, encode, infor, count);
        }
        
        endTime = clock();
        cout << "Decode time:" << (endTime - startTime) / 10.0<< endl;
    }
    

    delete[] data;
    delete[] encode;
    return 0;
}

void get_data(uint *data, uint mode)
{
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

uint calculate_bits(uint num)
{
    return (uint)floor(log(num) / log(2)) + 1;
}

uint calculate_space(uint *data) {
    // delta编码由两部分构成，对数据的位数进行gamma编码，和去除数据最高位
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

void encode_delta_piece(uint *data, uint *encode, uint *infor) {
    uint p = 0, shift = 32, q = 1;
    for(int i = 0; i < count ; i++) {
        if(infor[q] == i) {
            // 即当前数据处于断点位置，记录编码指针和整数内部偏移
            infor[++q] = p;
            infor[++q] = shift;
            ++q;
        }

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

void decode_data(uint *data, uint *encode, uint mode)
{
    uint p = 0, shift = 32, res = 0;
    uint cnt = 0;
    switch(mode)
    {
        case 1:
        {
            for(int i = 0; i < count ; i++)
            {
                res += decode_delta(encode, p, shift);
                if(res == data[i])
                    cnt++;
                else{
                    cout << i << endl;
                    break;
                }
            }
            break;
        }
        case 2:
        {
            for(int i = 0; i < count ; i++)
            {
                uint temp = decode_delta(encode, p, shift);
                if(data[i] == temp)
                    cnt++;
                else{
                    cout << i << endl;
                    break;
                }
            }
            break;
        }
        default :
            cout << "invalid mode!" << endl;
    }
    cout << "OK:" << cnt << endl;
}

void decode_piece(uint *data, uint *encode, uint *infor, uint data_size)
{
    uint inf = 0;                        // 信息数组的指针
    uint res = 0;
    uint p = 0, shift = 32;
    uint pre = 0;
    uint cnt = 0;
    for(int i = 0; i < data_size; i++)
    {
        if(i == infor[inf * 3 + 1])
        {
            res = decode_delta(encode, p, shift);
            inf++;
        }
        else
            res = pre + decode_delta(encode, p, shift);

        pre = res;
        if(res == data[i])
            cnt++;
        else{
            cout << "There is something wrong in " << i << endl;
            break;
        }
    }
    // cout << "OK!:" << cnt << endl;
}
