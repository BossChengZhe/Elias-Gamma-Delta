using namespace std;

#define uint unsigned int

uint get_lowbits(uint num, uint l)
{
    uint temp = (1U << l) - 1;
    return temp & num;
}

void store_low_bits(uint *low_bits, uint data_size)
{
    
}