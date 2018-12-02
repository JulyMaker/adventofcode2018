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

inline ostream&     RED(ostream& _Ostr)
{
    _Ostr.flush();
    SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_INTENSITY);
    return (_Ostr);
}
inline ostream&     GREEN(ostream& _Ostr)
{
    _Ostr.flush();
    SetConsoleTextAttribute(hStdOut, FOREGROUND_GREEN);
    return (_Ostr);
}
inline ostream&     YELLOW(ostream& _Ostr)
{
    _Ostr.flush();
    SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    return (_Ostr);
}
inline ostream&     RESET(ostream& _Ostr)
{
    _Ostr.flush();
    SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    return (_Ostr);
}

// GARLAND!
struct Garland
{
    int length;
    Garland(int length) : length(length) {/**/}
};
inline Garland GARLAND(int length)
{
    return Garland(length);
}
inline ostream& operator<<(ostream& os, const Garland& g)
{
    os << YELLOW << "**";
    for( int i=0; i<g.length; ++i)
    {
        os << RED << "o" << YELLOW << "*" << GREEN << "o" << YELLOW << "*";
    }
    os << "*" << RESET;
    return os;
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
        cout << GREEN << "PASS" << RESET;
    }
    else
    {
        cout << RED << "FAIL" << RESET << ": should be " << shouldbe << ", result was " << result;
    }

    cout << endl;

    return (result == shouldbe);
}

template<typename TResult>
void gogogo(TResult result)
{
    cout << "day" << gday << ",p" << gpart << ": FINAL RESULT: " << YELLOW << result << RESET << endl;

    if (gpart == 2)
    {
        gday++;
        gpart = 1;
    }
    else
        gpart++;

    gtest = 1;
    cout << "\n" << GARLAND(4) << endl;
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


// -------------------------------------------------------------------
int day2(const stringlist& lines)
{
    int numdoubles = 0;
    int numtriples = 0;

    for (auto line : lines)
    {
        const int numletters = 26;
        int seencount[numletters];
        memset(seencount, 0, sizeof(seencount));

        for (auto c : line)
        {
            int ix = c - 'a';
            seencount[ix]++;
        }

        int dub = 0;
        int trip = 0;
        for (int c = 0; c < numletters; ++c)
        {
            if (seencount[c] == 2)
                dub = 1;
            else if (seencount[c] == 3)
                trip = 1;

            if (dub + trip == 2)
                break;
        }
        numdoubles += dub;
        numtriples += trip;
    }

    return numdoubles * numtriples;
}

// returns the index of characters that are mismatched, or <0 if strings don't differ by a single char
int d2_compare(const string& a, const string& b)
{
    int diffpos = -1;

    auto ita = a.begin();
    auto itb = b.begin();
    for (; ita != a.end(); ++ita, ++itb)
    {
        if (*ita != *itb)
        {
            // >1 difference
            if (diffpos >= 0)
                return -1;

            diffpos = distance(a.begin(), ita);
        }
    }

    return diffpos;
}

string day2_2(const stringlist& lines)
{
    for (auto itline = lines.begin(); itline != lines.end(); ++itline)
    {
        const string& teststr = *itline;
        for (auto ittest = lines.begin(); ittest != itline; ++ittest)
        {
            int diff = d2_compare(teststr, *ittest);
            if (diff < 0)
                continue;

            // found it!!
            {
                string common = teststr.substr(0, diff);
                common += teststr.substr(diff + 1);
                return common;
            }
        }
    }

    return "oh dear";
}

// -------------------------------------------------------------------


int main()
{
    initcolours();

    cout << GARLAND(2) << "  advent of code 2018  " << GARLAND(2) << endl;

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

    test(12, day2(stringlist::fromstring("abcdef\nbababc\nabbcde\nabcccd\naabcdd\nabcdee\nababab")));
    gogogo(day2(stringlist::fromfile("day2.txt")));

    test<string>("fgij", day2_2(stringlist::fromstring("abcde\nfghij\nklmno\npqrst\nfguij\naxcye\nwvxyz")));
    gogogo(day2_2(stringlist::fromfile("day2.txt")));

    return 0;
}

