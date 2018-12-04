#include "harness.h"

#include <ctime>
#include <set>
#include <vector>


// -------------------------------------------------------------------

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

const int d3_clothsz = 1000;

int32_t d3_mark(int16_t* pcloth, int claim, int left, int top, int w, int h, bool* clean)
{
    _ASSERT(left + w <= d3_clothsz);
    _ASSERT(top + h <= d3_clothsz);

    int32_t newoverlap = 0;
    bool overlap = false;

    int16_t* pcurr = pcloth + (top * d3_clothsz) + left;
    for (int y = top; y < top + h; ++y)
    {
        for (int x = left; x < left + w; ++x, ++pcurr)
        {
            auto curr = *pcurr;
            if (curr == 0 || curr == claim)
            {
                *pcurr = claim;
            }
            else if (curr > 0)
            {
                newoverlap++;
                *pcurr = -1;
                overlap = true;
            }
            else
                overlap = true;
        }

        pcurr += d3_clothsz - w;
    }

    if (!overlap && clean)
        *clean = true;

    return newoverlap;
}

int32_t day3(const stringlist& input)
{
    int16_t* pclothbuf = new int16_t[d3_clothsz * d3_clothsz];
    memset(pclothbuf, 0, d3_clothsz * d3_clothsz);

    int32_t overlap = 0;
    for (auto line : input)
    {
        // #1 @ 1,3: 4x4
        istringstream is(line);
        char hash, at, comma, colon, by;
        int claim, left, top, width, height;
        is >> hash >> claim >> ws >> at >> ws >> left >> comma >> top >> colon >> ws >> width >> by >> height;
        overlap += d3_mark(pclothbuf, claim, left, top, width, height, NULL);
    }

    delete[] pclothbuf;
    return overlap;
}

int32_t day3_2(const stringlist& input)
{
    int16_t* pclothbuf = new int16_t[d3_clothsz * d3_clothsz];
    memset(pclothbuf, 0, d3_clothsz * d3_clothsz);

    // fill
    for (auto line : input)
    {
        // #1 @ 1,3: 4x4
        istringstream is(line);
        char hash, at, comma, colon, by;
        int claim, left, top, width, height;
        is >> hash >> claim >> ws >> at >> ws >> left >> comma >> top >> colon >> ws >> width >> by >> height;
        d3_mark(pclothbuf, claim, left, top, width, height, NULL);
    }
    // find clean
    int cleanclaim = -1;
    for (auto line : input)
    {
        // #1 @ 1,3: 4x4
        istringstream is(line);
        char hash, at, comma, colon, by;
        int claim, left, top, width, height;
        is >> hash >> claim >> ws >> at >> ws >> left >> comma >> top >> colon >> ws >> width >> by >> height;
        bool clean = false;
        d3_mark(pclothbuf, claim, left, top, width, height, &clean);

        if (clean)
        {
            cleanclaim = claim;
            break;
        }
    }

    delete[] pclothbuf;
    return cleanclaim;
}

// -------------------------------------------------------------------

int main()
{
    initcolours();
    srand((unsigned int)time(0));

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

    test(4, day3(stringlist::fromstring("#1 @ 1,3: 4x4\n#2 @ 3, 1: 4x4\n#3 @ 5, 5: 2x2")));
    gogogo(day3(stringlist::fromfile("day3.txt")));

    test(3, day3_2(stringlist::fromstring("#1 @ 1,3: 4x4\n#2 @ 3, 1: 4x4\n#3 @ 5, 5: 2x2")));
    gogogo(day3_2(stringlist::fromfile("day3.txt")));

    // animate snow falling behind the characters in the console until someone presses a key
    return twinkleforever();
}

