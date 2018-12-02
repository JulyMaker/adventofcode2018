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

HANDLE hStdOut;
void initcolours()
{
    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
}
void RED()
{
    cout << flush;
    SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_INTENSITY);
}
void GREEN()
{
    cout << flush;
    SetConsoleTextAttribute(hStdOut, FOREGROUND_GREEN);
}
void YELLOW()
{
    cout << flush;
    SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
}
void RESET()
{
    cout << flush;
    SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}





class stringlist : public list<string>
{
public:

    static stringlist fromstring(const string& str)
    {
        istringstream is(str);
        return fromistream(is);
    }

    static stringlist fromfile(const string& fname)
    {
        ifstream ifs ("data/" + fname);
        return fromistream(ifs);
    }

    static stringlist fromistream(istream& is)
    {
        stringlist lst;
        string s;
        while (getline(is, s))
            lst.push_back(s);

        return lst;
    }
};


int gday = 1;
int gpart = 1;
int gtest = 1;

template<typename TResult>
bool test(TResult result, TResult shouldbe)
{
    cout << "day" << gday << ",p" << gpart << ": test " << gtest << ": ";
    gtest++;

    if (result == shouldbe)
    {
        GREEN();
        cout << "PASS";
        RESET();
    }
    else
    {
        RED();
        cout << "FAIL";
        RESET();
        cout << ": should be " << shouldbe << ", result was " << result;
    }

    cout << endl;

    return (result == shouldbe);
}

template<typename TResult>
void gogogo(TResult result)
{
    cout << "day" << gday << ",p" << gpart << ": FINAL RESULT: ";
    YELLOW();
    cout << result << endl;
    RESET();

    if (gpart == 2)
    {
        gday++;
        gpart = 1;
    }
    else
        gpart++;

    gtest = 1;
    cout << "--=--=--=--=--=--=--=--=--=--=--=--=--=--\n" << endl;
}



int day1(const stringlist& lines)
{
    int acc = 0;
    for (auto line : lines)
    {
        int iline = stoi(line);
        acc += iline;
    }
    return acc;
}

int day1_2(const stringlist& lines)
{
    set<int> seen;

    int acc = 0;
    seen.insert(acc);

    bool seentwice = false;
    while (!seentwice)
    {
        for (auto line : lines)
        {
            int iline = stoi(line);
            acc += iline;

            if (!seentwice)
            {
                auto found = seen.find(acc);
                if (found != seen.end())
                {
                    seentwice = true;
                    return acc;
                }
                
                seen.insert(acc);
            }
        }
    }
    return -11111111;
}


int main()
{
    initcolours();

    cout << "o*o*o*O  advent of code 2018  O*o*o*o\n\n" << endl;

    test(3, day1(stringlist::fromstring("+1\n-2\n+3\n+1")));
    test(3, day1(stringlist::fromstring("+1\n+1\n+1")));
    test(0, day1(stringlist::fromstring("+1\n+1\n-2")));
    test(-6, day1(stringlist::fromstring("-1\n-2\n-3")));
    gogogo(day1(stringlist::fromfile("day1.txt")));

    test(0, day1_2(stringlist::fromstring("+1\n-1")));
    test(10, day1_2(stringlist::fromstring("+3\n+3\n+4\n-2\n-4")));
    test(5, day1_2(stringlist::fromstring("-6\n+3\n+8\n+5\n-6")));
    test(14, day1_2(stringlist::fromstring("+7\n+7\n-2\n-7\n-4")));
    gogogo(day1_2(stringlist::fromfile("day1.txt")));

    return 0;
}

