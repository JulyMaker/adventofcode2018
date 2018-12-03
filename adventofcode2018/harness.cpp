#include <algorithm>
#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <vector>


using namespace std;


#include <Windows.h>
#include <wincon.h>

HANDLE hStdIn;
HANDLE hStdOut;
void initcolours()
{
    hStdIn = GetStdHandle(STD_INPUT_HANDLE);
    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
}



int gday = 1;
int gpart = 1;
int gtest = 1;
