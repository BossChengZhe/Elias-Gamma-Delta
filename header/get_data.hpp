#include <fstream>

using namespace std;

#define uint unsigned int

void get_data(uint *data) 
{
    ifstream load_data("source\\data.txt");
    uint temp = 0, i = 0;
    while (load_data >> temp)
    {
        data[i++] = temp;
    }
    load_data.close();
}