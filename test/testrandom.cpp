#include <time.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

int main()
{
    for(int j = 0; j < 1; j++)
    {
	srand((int)time(0));
    for(int i = 0; i < 10; i++)
    {
	cout << rand()%100 << endl;
    }
    cout << "----------------------------------------------------" << endl;
    }
    
    return 1;
}
