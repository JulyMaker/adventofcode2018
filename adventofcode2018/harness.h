#pragma once

#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <string>

#include <Windows.h>
#include <wincon.h>


using namespace std;


// ---- COLOUR SHIZ ----
extern HANDLE hStdIn;
extern HANDLE hStdOut;
void initcolours();


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
inline ostream&     GREY(ostream& _Ostr)
{
    _Ostr.flush();
    SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
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
    Garland(int length) : length(length) {/**/ }
};
inline Garland GARLAND(int length)
{
    return Garland(length);
}
inline ostream& operator<<(ostream& os, const Garland& g)
{
    os << YELLOW << "**";
    for (int i = 0; i<g.length; ++i)
    {
        os << RED << "o" << YELLOW << "*" << GREEN << "o" << YELLOW << "*";
    }
    os << "*" << RESET;
    return os;
}




// -----   STRING LISTS  -----
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
        ifstream ifs("data/" + fname);
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

inline string stringfromfile(const string& fname)
{
    string str;

    ifstream ifs("data/" + fname);
    ifs.seekg(0, ios::end);
    auto size = ifs.tellg();
    if (size < 0)
        throw "couldn't discover file size o_O";

    str.reserve((size_t)size);
    ifs.seekg(0, ios::beg);

    str.assign((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());

    return str;
}



// ----- day harness -----
extern int gday;
extern int gpart;
extern int gtest;

template<typename TResult>
bool test(TResult shouldbe, TResult result)
{
    cout << "day" << gday << ",p" << gpart << ": test " << gtest << ": ";
    gtest++;

    if (result == shouldbe)
    {
        cout << GREEN << "PASS" << RESET;
    }
    else
    {
        cout << RED << "FAIL" << RESET << ": should be " << shouldbe << ", result was " << result << "\n";
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
        cout << "\n" << GARLAND(4) << "\n" << endl;
    }
    else
    {
        gpart++;
        cout << endl;
    }

    gtest = 1;
}

void skip(const char* message = "cos it's really slow!");

#define nest(...)
#define nonono(expr) skip()

#ifdef _DEBUG
#define nononoD(expr) skip()
#else
#define nononoD(expr) gogogo(expr)
#endif




int twinkleforever();

