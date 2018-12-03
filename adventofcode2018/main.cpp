#include "harness.h"

#include <set>
#include <vector>

#include <conio.h>


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

const WORD kwhite = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
const WORD kgrey = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
const wchar_t* ktwinkles = L"-+x┼x+- ";
const WORD ktwincols[] = { kgrey, kwhite, kgrey, kwhite, kwhite, kgrey, kgrey, kgrey };
COORD zerozero = { 0, 0 };
COORD oneXone = { 1, 1 };

struct Sparkle
{
    COORD pos;
    int t;

    Sparkle() {/**/}
    Sparkle(COORD p) : pos(p), t(-1)  {/**/}

    bool tick()
    {
        t += 1;
        if (!ktwinkles[t]) return false;

        CHAR_INFO c;
        c.Char.UnicodeChar = ktwinkles[t];
        c.Attributes = ktwincols[t];
        SMALL_RECT rect = { pos.X, pos.Y, 3000, 3000 };
        WriteConsoleOutput(hStdOut, &c, oneXone, zerozero, &rect);

        return true;
    }
};

int twinkleforever()
{
    COORD zerozero = { 0, 0 };

    // step 1. read back current screen
    CONSOLE_SCREEN_BUFFER_INFOEX screeninfo;
    screeninfo.cbSize = sizeof(screeninfo);
    if (!GetConsoleScreenBufferInfoEx(hStdOut, &screeninfo))
    {
        cerr << "FAILED to read console info: " << GetLastError() << endl;
        return 1;
    }
    size_t bufsize = screeninfo.dwSize.X * screeninfo.dwSize.Y;
    CHAR_INFO *pscreenbuf = new CHAR_INFO[bufsize];
    SMALL_RECT readregion = screeninfo.srWindow;
    if (!ReadConsoleOutput(hStdOut, pscreenbuf, screeninfo.dwSize, zerozero, &readregion))
    {
        cerr << "FAILED to read from console: " << GetLastError() << endl;
        return 1;
    }

    // step 2: make a big ol' list of all the coords that are free
    vector<COORD> free;
    int wndwidth = screeninfo.srWindow.Right - screeninfo.srWindow.Left;
    int wndheight = screeninfo.srWindow.Bottom - screeninfo.srWindow.Top;
    size_t i = 0;
    COORD pos = { 0,0 };
    for (; pos.Y < wndheight; ++pos.Y)
    {
        for (pos.X = 0; pos.X < wndwidth; ++pos.X, ++i)
        {
            if (pscreenbuf[i].Char.UnicodeChar == L' ')
            {
                free.push_back(pos);
            }
        }
    }

    list<Sparkle> sparkles;
    uint32_t frame = 0;
    for (;; ++frame)
    {
        // add a new sparkle
        size_t ipos = rand() % free.size();
        Sparkle newsparkle(free[ipos]);
        sparkles.push_back(newsparkle);

        // tick all the sparkles
        for (auto it = sparkles.begin(); it != sparkles.end(); ++it)
        {
            if (!it->tick())
            {
                it = sparkles.erase(it);
            }
        }

        if (_kbhit())
            break;

        Sleep(150);
    }

    return 0;
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
    nononoD(day1_2(stringlist::fromfile("day1.txt")));

    test(12, day2(stringlist::fromstring("abcdef\nbababc\nabbcde\nabcccd\naabcdd\nabcdee\nababab")));
    gogogo(day2(stringlist::fromfile("day2.txt")));

    test<string>("fgij", day2_2(stringlist::fromstring("abcde\nfghij\nklmno\npqrst\nfguij\naxcye\nwvxyz")));
    gogogo(day2_2(stringlist::fromfile("day2.txt")));

    // animate snow falling behind the characters in the console until someone presses a key
    return twinkleforever();
}

