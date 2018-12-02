#include "harness.h"

#include <set>



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

    // TODO: pri1: animate snow falling behind the characters in the console until someone presses ESC

    return 0;
}

