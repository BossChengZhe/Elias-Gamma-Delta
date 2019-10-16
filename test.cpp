#include <iostream>
using namespace std;

int main()
{
    int x[15] = {1, 3, 5, 7, 1, 2, 4, 6, 8, 10, 2, 5, 6, 7, 9 };
    int *infor = new int[9]();
    int q = 3, pre = x[0], te = pre;
    infor[0] = 0;
    for(int i = 0; i < 14 ; i++) {
        if(pre > x[i + 1]) {
            infor[q++] = i + 1;
            infor[q++] = 0;
            infor[q++] = 0;
            pre = x[i + 1];
        }
        else {
            te = x[i + 1];
            x[i + 1] -= pre;
            pre = te;
        }
    }
    for(int i = 0; i < 3 ; i++)
    {
        cout << infor[i * 3] << " ";
    }
}