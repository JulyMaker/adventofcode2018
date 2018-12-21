#include "harness.h"

#include <algorithm>
#include <ctime>
#include <iomanip>
#include <map>
#include <numeric>
#include <set>
#include <unordered_set>
#include <vector>

#include "day15.h"


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
    const int max_accum = 1000000;
    vector<uint8_t> seen(2 * max_accum, 0);

    int acc = 0;
    seen[max_accum] = 1;

    for (;;)
    {
        for (auto line : lines)
        {
            int iline = stoi(line);
            acc += iline;
            _ASSERT(acc < max_accum);
            _ASSERT(-acc < max_accum);

            if (seen[acc + max_accum])
                return acc;

            seen[acc + max_accum] = 1;
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
ptrdiff_t d2_compare(const string& a, const string& b)
{
    ptrdiff_t diffpos = -1;

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
            auto diff = d2_compare(teststr, *ittest);
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

struct D4Day
{
    int8_t month;
    int8_t day;
    int16_t guard;
    uint8_t minute[60]; // 1=asleep,0=awake

    D4Day() {/**/ }
    D4Day(int m, int d, int g) : month((int8_t)m), day((int8_t)d), guard((int16_t)g)
    {
        memset(minute, 0, sizeof(minute));
    }
};

int d4_find_sleepiest_guard(const vector<D4Day>& days)
{
    // build map of guard -> sleepiness
    map<int, int> guard_sleep;
    for (auto day : days)
    {
        auto itgs = guard_sleep.find(day.guard);
        if (itgs == guard_sleep.end())
            itgs = guard_sleep.insert(make_pair(day.guard, 0)).first;

        itgs->second += accumulate(day.minute, day.minute + 60, 0);
    }

    // find the guard who's asleep the most
    int guard = -1;
    int maxsleep = -1;
    for (auto kv : guard_sleep)
    {
        if (kv.second > maxsleep)
        {
            guard = kv.first;
            maxsleep = kv.second;
        }
    }

    return guard;
}

int d4_find_sleepiest_minute(const vector<D4Day>& days, int guard)
{
    uint8_t minutes[60];
    memset(minutes, 0, sizeof(minutes));

    for (auto day : days)
    {
        if (day.guard != guard)
            continue;

        for (int i = 0; i < 60; ++i)
        {
            minutes[i] += day.minute[i];
        }
    }

    auto itsleepy = max_element(minutes, minutes + 60);

    return (int)distance(minutes, itsleepy);
}


void d4_load(stringlist& input, vector<D4Day>& outdays)
{
    // input not date sorted
    input.sort();

    D4Day* pday = NULL;
    int sleepmin = -1;

    for (auto line : input)
    {
        // [1518-11-01 00:00] Guard #10 begins shift
        // [1518-11-01 00:05] falls asleep
        // [1518-11-01 00:25] wakes up
        istringstream is(line);
        int month, day, hour, minute;
        char dummy;
        is.ignore(6);
        is >> month >> dummy >> day >> ws >> hour >> dummy >> minute >> dummy >> ws;
        string event(std::istreambuf_iterator<char>(is), {});   // read the rest of the buffer into a string. could also just use substr...

        if (event[0] == 'G')
        {
            _ASSERT(sleepmin < 0);
            istringstream isev(event);
            isev.ignore(7);
            int guard;
            isev >> guard;
            outdays.push_back(D4Day(month, day, guard));
            pday = &outdays.back();
        }
        else if (event[0] == 'f')
        {
            _ASSERT(sleepmin < 0);
            sleepmin = minute;
        }
        else
        {
            _ASSERT(event[0] == 'w');
            _ASSERT(sleepmin >= 0);
            _ASSERT(sleepmin < 60);
            _ASSERT(minute <= 60);
            _ASSERT(minute > 0);
            fill(pday->minute + sleepmin, pday->minute + minute, 1);
            sleepmin = -1;
        }
    }
}

int day4(stringlist& input)
{
    vector<D4Day> days;
    d4_load(input, days);

    int sleepy = d4_find_sleepiest_guard(days);
    int sleepiest_minute = d4_find_sleepiest_minute(days, sleepy);

    return sleepy * sleepiest_minute;
}

int day4_2(stringlist& input)
{
    vector<D4Day> days;
    d4_load(input, days);

    map<int, vector<uint8_t> > guard_minutes;
    for (auto& day : days)
    {
        auto itgm = guard_minutes.find(day.guard);
        if (itgm == guard_minutes.end())
        {
            itgm = guard_minutes.emplace(make_pair(day.guard, vector<uint8_t>(60, 0))).first;
        }

        for (int i = 0; i < 60; ++i)
            itgm->second[i] += day.minute[i];
    }

    int guard = -1;
    int minute = -1;
    int sleepiness = -1;
    for (auto g_mins : guard_minutes)
    {
        // find the sleepiest minute
        auto itsleepy = max_element(g_mins.second.begin(), g_mins.second.end());
        if (*itsleepy > sleepiness)
        {
            guard = g_mins.first;
            sleepiness = *itsleepy;
            minute = (int)distance(g_mins.second.begin(), itsleepy);
        }
    }

    return guard * minute;
}

// -------------------------------------------------------------------

// v2, foregoing stl containers so we can include it in a debug run!
int day5(const string& s)
{
    auto itread = s.begin();

    char* sout = new char[s.length() + 1];
    memset(sout, 0, s.length() + 1);
    auto itbackch = sout;

    auto itend = s.end();
    while (itread != itend)
    {
        if (sout[0] != 0)
        {
            char nextch = *itread;
            char annihilate = *itbackch ^ 0x20;
            if (nextch != annihilate)
            {
                ++itbackch;
                *itbackch = nextch;
                ++itread;
            }
            else
            {
                *itbackch = '\0';       // not technically necessary, but nice for debugging
                if (itbackch != sout)
                    --itbackch;

                ++itread;
            }
        }
        else
        {
            // copy first char over
            *itbackch = *itread;
            ++itread;
        }
    }

    int length = (*sout) ? (int)(itbackch - sout + 1) : 0;

    delete[] sout;
    return length;
}

// v1 - quite fast, but not fast enough in debug config
int day5_o(string& s)
{
    // backwards for less copying

    // iterator overhead is crippling in debug builds; let's try it without!
    // RESULT: ~2x speedup. remaining time mostly in string.erase
    //   THINKS: maybe do something clever with batching contiguous reactions? doesn't look like it'll be much faster tho...
    char* prbegin = &s.back();
    char* prend = (&s.front() - 1);

    auto itbegin = s.begin();

    char danger = 0;
    for (auto it = prbegin; it != prend; --it)
    {
        if (*it == danger)
        {
            // move back to previous
            ++it;
            // erase it and it's next (this is all bakcwards, so we actually moved fwd 1 then erase from 2 back)
            auto itc = itbegin + (it - prend);
            s.erase(itc - 2, itc);
            prbegin -= 2;

            // move back another if we can, so we collapse any outer pair
            if (it != prbegin)
                --it;

            if (prbegin == prend)
                break;
        }

        danger = *it;
        danger ^= 0x20; // flip case yeah
    }

    return (int)(prbegin - prend);
}

int day5_2(const string& input)
{
    int min = 0x7fffff;
    for (char testc = 'a'; testc <= 'z'; ++testc)
    {
        string tests(input);
        tests.erase(remove_if(tests.begin(), tests.end(),
            [testc](char c) { return testc == (c | 0x20); }),
            tests.end());

        int collapsed = day5(tests);
        if (collapsed < min)
            min = collapsed;
    }

    return min;
}

// -------------------------------------------------------------------

struct D6Point
{
    int x, y;
    int area;
    int id;
    bool infinite;


    D6Point() { /**/ }
    D6Point(const string& line, int _id) : area(0), id(_id), infinite(false)
    {
        char comma;
        istringstream is(line);
        is >> x >> comma >> ws >> y;
    }

    int distfrom(int ox, int oy) const
    {
        return abs(ox - x) + abs(oy - y);
    }
};

int day6(const stringlist& input)
{
    // load input
    vector<D6Point> points;
    points.reserve(input.size());
    int id = 1;
    for (auto& line : input)
    {
        points.emplace_back(line, id);
        id++;
    }

    // scan points to find extents
    int minx = 1000000, miny = 1000000;
    int maxx = -1, maxy = -1;
    for (auto& point : points)
    {
        minx = min(minx, point.x);
        miny = min(miny, point.y);
        maxx = max(maxx, point.x);
        maxy = max(maxy, point.y);
    }

    // spin over the space and classify each point
    for (int y = miny; y <= maxy; ++y)
    {
        bool infinitey = (y == miny) || (y == maxy);

        for (int x = minx; x <= maxx; ++x)
        {
            D6Point* nearest = NULL;
            int mindist = 1000000000;

            for (auto& point : points)
            {
                int dist = point.distfrom(x, y);
                if (dist < mindist)
                {
                    mindist = dist;
                    nearest = &point;
                }
                else if (dist == mindist)
                {
                    nearest = NULL;
                }
            }

            if (nearest)
            {
                bool infinite = infinitey || (x == minx) || (x == maxx);

                nearest->infinite |= infinite;
                nearest->area++;
            }
        }
    }

    // now find the biggest non-infinite area
    const D6Point* nearest = NULL;
    int biggestarea = -1;
    for (auto point : points)
    {
        if (!point.infinite && point.area > biggestarea)
        {
            nearest = &point;
            biggestarea = point.area;
        }
    }

    return biggestarea;
}

int day6_2(const stringlist& input, int safedist)
{
    // load input
    vector<D6Point> points;
    points.reserve(input.size());
    int id = 1;
    for (auto& line : input)
    {
        points.emplace_back(line, id);
        id++;
    }

    // scan points to find extents
    int minx = 1000000, miny = 1000000;
    int maxx = -1, maxy = -1;
    for (auto& point : points)
    {
        minx = min(minx, point.x);
        miny = min(miny, point.y);
        maxx = max(maxx, point.x);
        maxy = max(maxy, point.y);
    }

    // whip over the whole space, then over all the points to measure safety
    // we going O(n^3) here o_O
    int safepoints = 0;
    for (int x = minx; x <= maxx; ++x)
    {
        for (int y = miny; y <= maxy; ++y)
        {
            int totaldist = 0;
            for (auto& point : points)
            {
                totaldist += point.distfrom(x, y);
                if (totaldist > safedist)
                    break;
            }
            if (totaldist < safedist) {
                safepoints++;
            }
        }
    }

    return safepoints;
}

// -------------------------------------------------------------------

struct D7Step
{
    bool ready;
    char name;
    int8_t totaltime;
    int8_t timeleft;
    vector<char> dependson;

    D7Step() {/**/ }
    D7Step(char _name, int8_t extratime) : ready(true), name(_name)
    {
        totaltime = 1 + (_name - 'A') + extratime;
        timeleft = totaltime;
    }

    void adddep(char dep)
    {
        dependson.push_back(dep);
        ready = false;
    }
};

void d7_loadsteps(const stringlist& input, int numsteps, vector<D7Step>& outsteps, int extratime = 0)
{
    outsteps.reserve(numsteps);
    for (int step = 0; step < numsteps; ++step)
    {
        outsteps.emplace_back(step + 'A', extratime);
    }

    // load the instructions
    for (auto& line : input)
    {
        // "Step A must be finished before step D can begin."
        char step;
        char dependson;
        istringstream is(line);
        is.ignore(10000, ' ');
        is >> dependson;
        is.seekg(30, ios_base::cur);
        is >> step;

        outsteps[step - 'A'].adddep(dependson);
    }
}


void d7_updateopen(vector<D7Step>& steps, const vector<bool>& completed, set<char>& open)
{
    // update our steps, adding any newly ready steps to our open set
    for (auto& step : steps)
    {
        if (step.ready)
            continue;

        bool readynow = true;
        for (char dep : step.dependson)
        {
            if (!completed[dep - 'A'])
            {
                readynow = false;
                break;
            }
        }

        if (readynow)
        {
            open.insert(step.name);
            step.ready = true;
        }
    }
}


string day7(const stringlist& input, int numsteps)
{
    vector<D7Step> steps;
    d7_loadsteps(input, numsteps, steps);

    // build the initial list of open steps
    set<char> open;
    for (auto& step : steps)
    {
        if (step.ready)
            open.insert(step.name);
    }

    vector<bool> completed(numsteps, false);

    // build the sleigh!
    string order;
    while (!open.empty())
    {
        auto itnext = open.begin();
        char next = *itnext;
        open.erase(itnext);

        order.push_back(next);
        completed[next - 'A'] = true;

        d7_updateopen(steps, completed, open);
    }

    return order;
}


int day7_2(const stringlist& input, int numsteps, int numworkers, int extratime)
{
    vector<D7Step> steps;
    d7_loadsteps(input, numsteps, steps, extratime);

    // build the initial list of open steps
    set<char> open;
    for (auto& step : steps)
    {
        if (step.ready)
            open.insert(step.name);
    }

    vector<bool> completed(numsteps, false);

    // build the sleigh!
    string order;
    int time = 0;
    vector<D7Step*> workers(numworkers, 0);
    int numworking = 0;
    while (!open.empty() || numworking)
    {
        // find work
        for (auto itworker = workers.begin(); itworker != workers.end(); ++itworker)
        {
            if (!*itworker && !open.empty())
            {
                auto itnext = open.begin();
                char next = *itnext;
                open.erase(itnext);

                *itworker = &steps[next - 'A'];
                ++numworking;
            }
        }

        // do work
        for (auto itworker = workers.begin(); itworker != workers.end(); ++itworker)
        {
            // do some work if we have some
            if (*itworker)
            {
                D7Step* active = *itworker;
                active->timeleft--;
                if (active->timeleft == 0)
                {
                order.push_back(active->name);
                completed[active->name - 'A'] = true;
                *itworker = NULL;
                --numworking;

                d7_updateopen(steps, completed, open);
                }
            }
        }

        time++;
    }

    return time;
}

// -------------------------------------------------------------------

int d8_summetadata(istream& is)
{
    int numchildren, nummeta;
    is >> numchildren >> ws >> nummeta >> ws;

    int metatotal = 0;
    for (int c = 0; c < numchildren; ++c)
    {
        metatotal += d8_summetadata(is);
    }

    for (int m = 0; m < nummeta; ++m)
    {
        int meta;
        is >> meta >> ws;
        metatotal += meta;
    }

    return metatotal;
}

int day8(const string& input)
{
    istringstream is(input);
    return d8_summetadata(is);
}

struct D8Node
{
    vector<D8Node> children;
    vector<int> meta;
    int value;

    D8Node() {/**/ }
    D8Node(istream& is)
    {
        int numchildren, nummeta;
        is >> numchildren >> ws >> nummeta >> ws;

        children.reserve(numchildren);
        for (int i = 0; i < numchildren; ++i)
        {
            children.emplace_back(is);
        }

        meta.reserve(nummeta);
        for (int i = 0; i < nummeta; ++i)
        {
            int m;
            is >> m >> ws;
            meta.push_back(m);
        }

        value = 0;
        if (numchildren)
        {
            for (auto m : meta)
            {
                if (m <= numchildren)
                {
                    value += children[m - 1].value;
                }
            }
        }
        else
        {
            value = accumulate(meta.begin(), meta.end(), 0);
        }
    }
};

int day8_2(const string& input)
{
    istringstream is(input);
    D8Node root(is);
    return root.value;
}

// -------------------------------------------------------------------

void ringadvance(list<int>::iterator& it, list<int>& l, int dist)
{
    while (dist)
    {
        ++it;
        if (it == l.end())
            it = l.begin();

        --dist;
    }
}

void ringretreat(list<int>::iterator& it, list<int>& l, int dist)
{
    while (dist)
    {
        if (it == l.begin())
            it = l.end();
        --it;

        --dist;
    }
}


int64_t day9(const string& input )
{
    // 9 players; last marble is worth 25 points
    int nplayers, maxmarble;
    istringstream is(input);
    is >> nplayers;
    is.seekg(30, ios_base::cur);
    is >> maxmarble;

    vector<int64_t> playerscores(nplayers, 0);

    // play the game!
    list<int> marbles = { 0,1 };
    auto itcur = next(marbles.begin());
    int curplayer = 0;
    for (int marble = 2; marble <= maxmarble; ++marble)
    {
        if (marble % 23)
        {
            ringadvance(itcur, marbles, 2);
            itcur = marbles.insert(itcur, marble);
        }
        else
        {
            ringretreat(itcur, marbles, 7);

            playerscores[curplayer] += marble + *itcur;
            itcur = marbles.erase(itcur);
            if (itcur == marbles.end())
                itcur = marbles.begin();
        }

        curplayer = (curplayer + 1) % nplayers;
    }

    return *max_element(playerscores.begin(), playerscores.end());
}

// -------------------------------------------------------------------

// (see day10 project - ../day10/ -- for day10!)

// -------------------------------------------------------------------

const int d11_ncells = 300 + 1;

struct D11Point
{
    int x, y, s;
    bool operator==(const D11Point& o) const
    {
        return x == o.x && y == o.y && s == o.s;
    }
};
ostream& operator<<(ostream& os, const D11Point& pt)
{
    os << '(' << pt.x << ',' << pt.y << ',' << pt.s << ')';
    return os;
}

int d11_calcpower(int gridnum, int x, int y)
{
    int rackid = x + 10;
    int64_t power = rackid * y;
    power += gridnum;
    power *= rackid;
    power = (power / 100) % 10;
    power -= 5;
    return (int)power;
}

void d11_fillgrid(int gridnum, int8_t* grid)
{
    for (int y = 1; y <= 300; ++y)
    {
        for (int x = 1; x <= 300; ++x)
        {
            grid[x + y*d11_ncells] = d11_calcpower(gridnum, x, y);
        }
    }
}

D11Point d11_findpowerfulest(const int8_t* grid, int size, int* outbestpower=NULL)
{
    D11Point bestpoint = { 0,0,size };
    int bestpower = -100000;
    for (int y = 1; y <= (300 + 1 - size); ++y)
    {
        for (int x = 1; x <= (300 + 1 - size); ++x)
        {
            const int8_t* pcells = grid + x + y*d11_ncells;
            int power = 0;
            for (int r = 0; r < size; ++r)
            {
                const int8_t* prow = pcells;

                int colsleft = size;
                while (colsleft >= 8)
                {
                    int p0 = prow[0] + prow[1];
                    int p1 = prow[2] + prow[3];
                    int p2 = prow[4] + prow[5];
                    int p3 = prow[6] + prow[7];
                    int pl = p0 + p1;
                    int pr = p2 + p3;
                    power += pl + pr;

                    colsleft -= 8;
                    prow += 8;
                }
                switch (colsleft)
                {
                case 7: power += prow[6];
                case 6: power += prow[5];
                case 5: power += prow[4];
                case 4: power += prow[3];
                case 3: power += prow[2];
                case 2: power += prow[1];
                case 1: power += prow[0];
                }

                pcells += d11_ncells;
            }

            if (power > bestpower)
            {
                bestpoint.x = x;
                bestpoint.y = y;
                bestpower = power;
            }
        }
    }

    if (outbestpower)
        *outbestpower = bestpower;

    return bestpoint;
}


D11Point day11(int gridnum)
{
    int8_t* grid = new int8_t[d11_ncells * d11_ncells];

    // fill the grid with cell powers
    d11_fillgrid(gridnum, grid);

    // seek out the biggest 3x3 square
    D11Point bestpoint = d11_findpowerfulest(grid, 3);
    
    delete[] grid;
    return bestpoint;
}

D11Point day11_2(int gridnum)
{
    int8_t* grid = new int8_t[d11_ncells * d11_ncells];

    // fill the grid with cell powers
    d11_fillgrid(gridnum, grid);

    // seek out the biggest 3x3 square
    int bestpower = -1000000;
    D11Point bestpoint;
    for (int size = 1; size < 300; ++size)
    {
        if ((size % 10) == 1)
            cout << '.' << flush;

        int power = -1000000;
        D11Point point = d11_findpowerfulest(grid, size, &power);

        if (power > bestpower)
        {
            bestpoint = point;
            bestpower = power;
        }
    }
    cout << '\r' << flush;

    delete[] grid;
    return bestpoint;
}

// -------------------------------------------------------------------

class D12Machine
{
    // rules ordering:   LSB->MSB   L2 L1 X R1 R2 
    bool rules[32];
    vector<bool> pots;
    int numnegpots;
    int numextrapots;

public:
    int64_t generation;

    D12Machine(const stringlist& input)
        : numnegpots(0)
        , numextrapots(0)
        , generation(0)
    {
        auto itin = input.begin();

        // initial state: #....##.#.#.####..#.######..##.#.########..#...##...##...##.#.#...######.###....#...##..#.#....##.##
        {
            const string& istate = *itin;
            auto itc = istate.begin() + 15;

            pots.resize(numnegpots + (istate.end() - itc) + numextrapots, false);

            for (int n=numnegpots; itc != istate.end(); ++itc, ++n)
            {
                bool plant = (*itc == '#');

                pots[n] = plant;
            }
        }
        ++itin;

        // skip blank line
        ++itin;

        // read the rules (assume everything is no pot by default)
        memset(rules, 0, sizeof(rules));
        {
            for (; itin != input.end(); ++itin)
            {
                const string& in = *itin;

                // ...## => #
                int id = 0;
                int mask = 1;
                for (int b = 0; b < 5; ++b, mask <<= 1)
                {
                    if (in[b] == '#') id |= mask;
                }

                rules[id] = (in[9] == '#');
            }
        }
    }

    void tick()
    {
        ++generation;

        if (pots[0] || pots[1] || pots[2] || pots[3])
        {
            pots.insert(pots.begin(), 4, false);
            numnegpots += 4;
        }
        auto numpots = pots.size();
        if (pots[numpots - 4] || pots[numpots - 3] || pots[numpots - 2] || pots[numpots-1])
        {
            pots.push_back(false);
            pots.push_back(false);
            pots.push_back(false);
            pots.push_back(false);
        }

        numpots = pots.size();
        vector<bool> npots(numpots, false);

        for (uint32_t i = 2; i < numpots-2; ++i)
        {
            int rule = 0;
            for (int b = 0, m=1; b < 5; ++b, m<<=1)
            {
                if (pots[i - 2 + b])
                    rule |= m;
            }

            npots[i] = rules[rule];
        }

        pots.swap(npots);
    }

    int calcscore() const
    {
        int score = 0;
        for (uint32_t i = 0; i < pots.size(); ++i)
        {
            if (pots[i])
            {
                score += i - numnegpots;
            }
        }
        return score;
    }

    friend ostream& operator<<(ostream& os, const D12Machine& m);
};

ostream& operator<<(ostream& os, const D12Machine& m)
{
    os << setfill(' ') << setw(4) << m.generation << setw(1) << ": ";
    for (auto pot : m.pots)
    {
        os << (pot ? '#' : '.');
    }
    return os;
}


int day12(const stringlist& input, int64_t generations)
{
    D12Machine machine(input);

    //cout << machine << endl;
    for (int64_t g = 0; g < generations; ++g)
    {
        machine.tick();
        // cout << machine << endl;
    }

    //cout << machine << endl;
    return machine.calcscore();
}

int64_t day12_2(const stringlist& input)
{
    D12Machine machine(input);

    //cout << machine << endl;
    for (int64_t g = 0; g < 80; ++g)
    {
        machine.tick();
        // cout << machine << endl;
    }
    //cout << machine << endl;
    for (int64_t g = 0; g < 10; ++g)
    {
        machine.tick();
        //cout << machine.calcscore() << '/' << machine << endl;
    }

    int64_t gens = 50ll * 1000ll * 1000ll * 1000ll;
    int64_t gensleft = gens - machine.generation;
    int64_t finalscore = gensleft * 15 + machine.calcscore();

    //cout << machine << endl;
    return finalscore;
}

// -------------------------------------------------------------------

struct D13Cart
{
    int x, y;
    int vx, vy;
    char track;
    int8_t nextchoice;

    D13Cart() {/**/}
    D13Cart( int x, int y ) : x(x), y(y), vx(0), vy(0) {/**/}
    D13Cart(int x, int y, char c) : x(x), y(y), nextchoice(0)
    {
        switch (c)
        {
        case '>': vx = 1; vy = 0; track = '-'; break;
        case '<': vx =-1; vy = 0; track = '-'; break;
        case '^': vx = 0; vy =-1; track = '|'; break;
        case 'v': vx = 0; vy = 1; track = '|'; break;
        default: _ASSERT(false && "wtf even is this cart?"); break;
        }
    }

    void turnleft()
    {
        auto ox = vx;
        vx = vy;
        vy = -ox;
    }
    void turnright()
    {
        auto ox = vx;
        vx = -vy;
        vy = ox;
    }
    void turnwherever()
    {
        switch (nextchoice)
        {
        case 0: turnleft(); break;
        case 1: /* straight on */  break;
        case 2: turnright(); break;
        }
        nextchoice = (nextchoice + 1) % 3;
    }

    void stop()
    {
        vx = vy = 0;
    }
    bool isstopped() const
    {
        return (vx == 0 && vy == 0);
    }

    bool operator==(const D13Cart& o) const
    {
        if (x == o.x && y == o.y)
            return true;

        return false;
    }
    char getAvatar() const
    {
        if (vx > 0) return '>';
        if (vx < 0) return '<';
        if (vy > 0) return 'v';
        if (vy < 0) return '^';
        return 'X';
    }
};
ostream& operator<<(ostream& os, const D13Cart& cart)
{
    os << cart.getAvatar() << ' ' << cart.x << ',' << cart.y;
    return os;
}

class D13System
{
    uint32_t width, height;
    char* map;
    vector<D13Cart> carts;

    D13Cart crashedcart;

public:

    D13System(const stringlist& input) : crashedcart( -1, -1 )
    {
        // find dimensions
        width = 0;
        for (auto& line : input)
            width = max(width, (uint32_t)line.length());
        height = (uint32_t)input.size();
        map = new char[width * height];
        memset(map, ' ', width * height);

        int y = 0;
        for (auto& line : input)
        {
            char* mapc = map + y * width;
            for (uint32_t x = 0; x < line.length(); ++x, ++mapc)
            {
                char c = line[x];
                *mapc = c;
                
                if (c == '>' || c == '<' || c == 'v' || c == '^')
                    carts.emplace_back(x, y, c);
            }
            ++y;
        }
    }
    ~D13System()
    {
        delete[] map;
    }

    const bool crash() const
    {
        return crashedcart.x >= 0;
    }

    const D13Cart& getCrash() const
    {
        _ASSERT(crash());
        return crashedcart;
    }

    const size_t numcarts() const
    {
        return carts.size();
    }
    const D13Cart& getLastCartStanding() const
    {
        _ASSERT(carts.size() == 1);
        return carts[0];
    }

    void cleanupCrash()
    {
        _ASSERT(crash());

        for (auto it = carts.begin(); it != carts.end(); ++it)
        {
            if (it->isstopped())
                map[it->x + it->y*width] = it->track;
        }

        carts.erase(remove_if(carts.begin(), carts.end(), [](auto& cart)->bool {return cart.isstopped(); }), carts.end());

        crashedcart.x = -1;
        crashedcart.y = -1;
    }


    void tick();
};

void D13System::tick()
{
    for (auto& cart : carts)
    {
        if (cart.isstopped())
            continue;

        // return the track to the previous state
        map[cart.x + cart.y * width] = cart.track;

        auto nx = cart.x + cart.vx;
        auto ny = cart.y + cart.vy;
        _ASSERT(nx >= 0);
        _ASSERT(ny >= 0);
        _ASSERT(nx < (int)width);
        _ASSERT(ny < (int)height);
        char nc = map[nx + ny * width];
        if (nc == '-' || nc == '|')
        {
            // continue straight
        }
        else if (nc == '/')
        {
            if (cart.vy < 0 || cart.vy > 0)
                cart.turnright();
            else
                cart.turnleft();
        }
        else if (nc == '\\')
        {
            if (cart.vy < 0 || cart.vy > 0)
                cart.turnleft();
            else
                cart.turnright();
        }
        else if (nc == '+')
        {
            cart.turnwherever();
        }
        else if (nc == '<' || nc == '>' || nc == '^' || nc == 'v' || nc == 'X')
        {
            // CRASH!
            crashedcart = D13Cart(nx, ny, cart.getAvatar());
            cart.track = nc;
            cart.x = nx;
            cart.y = ny;
            cart.stop();

            auto other = find_if(carts.begin(), carts.end(), [&](const D13Cart& c)->bool { return cart == c && (&c != &cart); });
            _ASSERT(other != carts.end());
            _ASSERT(!other->isstopped());
            other->stop();

            cart.track = other->track;

            //map[nx + ny * width] = 'X';
            continue;
        }
        else
        {
            _ASSERT(0 && "WTF is up with this track?");
        }

        cart.track = nc;
        cart.x = nx;
        cart.y = ny;
        map[nx + ny * width] = cart.getAvatar();
    }

    // make sure we process carts in the correct order - top->bottom, left->right
    sort(carts.begin(), carts.end(), [](const D13Cart& a, const D13Cart& b) -> bool
        {
            if (a.y < b.y) return true;
            if (a.y == b.y && a.x < b.x) return true;
            return false;
        });
}


D13Cart day13(const stringlist& input)
{
    D13System system(input);

    while (!system.crash())
    {
        system.tick();
    }

    return system.getCrash();
}

D13Cart day13_2(const stringlist& input)
{
    D13System system(input);

    while (system.numcarts() > 1)
    {
        while (!system.crash())
        {
            system.tick();
        }
        system.cleanupCrash();
    }

    return system.getLastCartStanding();
}

// -------------------------------------------------------------------

string day14(int input)
{
    vector<char> recipes{ '3', '7' };
    recipes.reserve(1000000);

    uint32_t elves[2] = { 0, 1 };

    while (recipes.size() < (uint32_t)(input + 10))
    {
        int r0score = recipes[elves[0]] - '0';
        int r1score = recipes[elves[1]] - '0';
        int newscore = r0score + r1score;

        if (newscore >= 10)
        {
            recipes.push_back((newscore / 10) + '0');
            if (recipes.size() == (uint32_t)(input + 10))
                break;

            recipes.push_back((newscore % 10) + '0');
        }
        else
            recipes.push_back(newscore + '0');

        elves[0] = (elves[0] + 1 + r0score) % recipes.size();
        elves[1] = (elves[1] + 1 + r1score) % recipes.size();
    }

    string last10(recipes.end() - 10, recipes.end());
    return last10;
}


bool d14_addrecipe(int r, vector<char>& recipes, const string& sought, uint32_t soughtlen)
{
    _ASSERT(r < 10);
    recipes.push_back(r + '0');

    if (recipes.size() >= soughtlen)
    {
        auto unfound = mismatch(sought.begin(), sought.end(), recipes.end() - soughtlen);
        if (unfound.first == sought.end())
            return true;
    }

    return false;
}

int day14_2(const char* input)
{
    string sought(input);
    uint32_t soughtlen = (uint32_t)sought.size();
    vector<char> recipes{ '3', '7' };
    recipes.reserve(1000000);

    uint32_t elves[2] = { 0, 1 };

    for (;; )
    {
        int r0score = recipes[elves[0]] - '0';
        int r1score = recipes[elves[1]] - '0';
        int newscore = r0score + r1score;

        if (newscore >= 10)
        {
            if (d14_addrecipe(newscore / 10, recipes, sought, soughtlen))
                return (uint32_t)recipes.size() - soughtlen;
        }

        if (d14_addrecipe(newscore % 10, recipes, sought, soughtlen))
            return (uint32_t)recipes.size() - soughtlen;

        elves[0] = (elves[0] + 1 + r0score) % recipes.size();
        elves[1] = (elves[1] + 1 + r1score) % recipes.size();
    }

    return 0;
}

// -------------------------------------------------------------------

struct D16Regs
{
    int r[4];

    D16Regs()
    {
        fill(r, r + 4, 0);
    }
    D16Regs(const string& def)
    {
        istringstream is(def);
        is.ignore(1000, '[');
        char comma;
        is >> skipws >> r[0] >> comma >> r[1] >> comma >> r[2] >> comma >> r[3];
    }

    bool operator==(const D16Regs& o) const
    {
        for (int i = 0; i < 4; ++i)
            if (r[i] != o.r[i])
                return false;
        return true;
    }
    bool operator!=(const D16Regs& o) const
    {
        for (int i = 0; i < 4; ++i)
            if (r[i] != o.r[i])
                return true;
        return false;
    }
};

struct D16Instr
{
    int op;
    int a, b, c;

    D16Instr(int op, int a, int b, int c) : op(op), a(a), b(b), c(c)
    { /**/ }
    D16Instr(const string& def)
    {
        istringstream is(def);
        is >> skipws >> op >> a >> b >> c;
        _ASSERT(a >= 0 && a < 4);
        _ASSERT(b >= 0 && b < 4);
        _ASSERT(c >= 0 && c < 4);
    }

    D16Regs addr(const D16Regs& in) 
    {
        D16Regs out = in;
        out.r[c] = in.r[a] + in.r[b];
        return out;
    }
    D16Regs addi(const D16Regs& in) 
    {
        D16Regs out = in;
        out.r[c] = in.r[a] + b;
        return out;
    }
    D16Regs mulr(const D16Regs& in) 
    {
        D16Regs out = in;
        out.r[c] = in.r[a] * in.r[b];
        return out;
    }
    D16Regs muli(const D16Regs& in) 
    {
        D16Regs out = in;
        out.r[c] = in.r[a] * b;
        return out;
    }
    D16Regs banr(const D16Regs& in) 
    {
        D16Regs out = in;
        out.r[c] = in.r[a] & in.r[b];
        return out;
    }
    D16Regs bani(const D16Regs& in) 
    {
        D16Regs out = in;
        out.r[c] = in.r[a] & b;
        return out;
    }
    D16Regs borr(const D16Regs& in) 
    {
        D16Regs out = in;
        out.r[c] = in.r[a] | in.r[b];
        return out;
    }
    D16Regs bori(const D16Regs& in) 
    {
        D16Regs out = in;
        out.r[c] = in.r[a] | b;
        return out;
    }
    D16Regs setr(const D16Regs& in) 
    {
        D16Regs out = in;
        out.r[c] = in.r[a];
        return out;
    }
    D16Regs seti(const D16Regs& in) 
    {
        D16Regs out = in;
        out.r[c] = a;
        return out;
    }
    D16Regs gtir(const D16Regs& in) 
    {
        D16Regs out = in;
        out.r[c] = (a > in.r[b]) ? 1 : 0;
        return out;
    }
    D16Regs gtri(const D16Regs& in) 
    {
        D16Regs out = in;
        out.r[c] = (in.r[a] > b) ? 1 : 0;
        return out;
    }
    D16Regs gtrr(const D16Regs& in) 
    {
        D16Regs out = in;
        out.r[c] = (in.r[a] > in.r[b]) ? 1 : 0;
        return out;
    }
    D16Regs eqir(const D16Regs& in) 
    {
        D16Regs out = in;
        out.r[c] = (a == in.r[b]) ? 1 : 0;
        return out;
    }
    D16Regs eqri(const D16Regs& in) 
    {
        D16Regs out = in;
        out.r[c] = (in.r[a] == b) ? 1 : 0;
        return out;
    }
    D16Regs eqrr(const D16Regs& in) 
    {
        D16Regs out = in;
        out.r[c] = (in.r[a] == in.r[b]) ? 1 : 0;
        return out;
    }
};


ostream& operator<<(ostream& os, const D16Regs& regs)
{
    os << "r0: " << regs.r[0] << "  r1: " << regs.r[1] << "  r2: " << regs.r[2] << "  r3: " << regs.r[3];
    return os;
}
ostream& operator<<(ostream& os, const D16Instr& ins)
{
    os << '<' << ins.op << ">  " << ins.a << ", " << ins.b << "  ->  " << ins.c;
    return os;
}



int day16(const stringlist& input)
{
    int numtriples = 0;

    for (auto itline = input.begin(); itline != input.end(); ++itline)
    {
        // give up when we hit a double newline
        if (itline->empty())
            break;

        D16Regs before(*itline);
        ++itline;
        D16Instr instr(*itline);
        ++itline;
        D16Regs after(*itline);
        ++itline;

        int numpossibles = 0;
        if (instr.addr(before) == after) ++numpossibles;
        if (instr.addi(before) == after) ++numpossibles;
        if (instr.mulr(before) == after) ++numpossibles;
        if (instr.muli(before) == after) ++numpossibles;
        if (instr.banr(before) == after) ++numpossibles;
        if (instr.bani(before) == after) ++numpossibles;
        if (instr.borr(before) == after) ++numpossibles;
        if (instr.bori(before) == after) ++numpossibles;
        if (instr.setr(before) == after) ++numpossibles;
        if (instr.seti(before) == after) ++numpossibles;
        if (instr.gtir(before) == after) ++numpossibles;
        if (instr.gtri(before) == after) ++numpossibles;
        if (instr.gtrr(before) == after) ++numpossibles;
        if (instr.eqir(before) == after) ++numpossibles;
        if (instr.eqri(before) == after) ++numpossibles;
        if (instr.eqrr(before) == after) ++numpossibles;

        if (numpossibles >= 3)
            ++numtriples;
    }

    return numtriples;
}

class we_sussed_it : public exception  { /**/ };

typedef D16Regs(__thiscall D16Instr::* d16_instr_fn)(const D16Regs &);
typedef vector<pair<const char*, d16_instr_fn>> d16_instr_possibles;
typedef vector<d16_instr_possibles> d16_possibles;

ostream& operator<<(ostream& os, const d16_possibles& possibles)
{
    for (int i = 0; i < 16; ++i)
    {
        const auto& idposs = possibles[i];
        cout << setw(2) << i << "  =>  ";

        for (auto& poss : idposs)
        {
            if (idposs.size() > 1 && &poss != &idposs.front())
                cout << " | ";

            cout << poss.first;
        }
        cout << '\n';
    }

    return os;
}


d16_instr_possibles::iterator d16_ruleout(d16_possibles& possibles, int id, const d16_instr_possibles::value_type& itop)
{
    // take a copy of the op we're erasing or we get ourselves into a right tizzy
    auto notop = itop;
    auto& idposs = possibles[id];
    auto itfound = find(idposs.begin(), idposs.end(), notop);
    if (itfound != idposs.end())
    {
        _ASSERT(idposs.size() != 1);
        itfound = idposs.erase(itfound);
    }
    else
        return idposs.end();

    if (idposs.size() == 1)
    {
        // rule that op out of every other id
        auto foundop = idposs.front();

        bool solved = true;
        for (int i = 0; i < 16; ++i)
        {
            if (id != i)
            {
                d16_ruleout(possibles, i, foundop);
                if (possibles[i].size() > 1)
                    solved = false;
            }
        }

        // because this can get recursive, our iterator can shift, so we just restart
        itfound = idposs.begin();

        if (solved)
            throw we_sussed_it();
    }

    return itfound;
}


int day16_2(const stringlist& input)
{

    d16_possibles possibles(16);
    for (int i = 0; i < 16; ++i)
    {
#define INSTR(opcode)   possibles[i].push_back( make_pair( #opcode, &D16Instr::opcode ))

        INSTR(addr);
        INSTR(addi);
        INSTR(mulr);
        INSTR(muli);
        INSTR(banr);
        INSTR(bani);
        INSTR(borr);
        INSTR(bori);
        INSTR(setr);
        INSTR(seti);
        INSTR(gtir);
        INSTR(gtri);
        INSTR(gtrr);
        INSTR(eqir);
        INSTR(eqri);
        INSTR(eqrr);

#undef INSTR
        _ASSERT(possibles[i].size() == 16);
    }
    
    bool solved = false;
    auto itline = input.begin();
    try
    {
        for (; itline != input.end(); ++itline)
        {
            // give up when we hit a double newline
            if (itline->empty())
                break;

            D16Regs before(*itline);
            ++itline;
            D16Instr instr(*itline);
            ++itline;
            D16Regs after(*itline);
            ++itline;

            // go through all of the possible operations we have left for this instruction
            auto& id_poss = possibles[instr.op];
            for (auto it = id_poss.begin(); it != id_poss.end(); )
            {
                auto name = it->first;
                auto fn = it->second;
                if ((instr.*fn)(before) != after)
                {
                    it = d16_ruleout(possibles, instr.op, *it);
                }
                else
                    ++it;
            }
        }
    }
    catch (we_sussed_it&)
    {
        solved = true;
    }

    if (!solved)
    {
        cerr << "couldn't crack it T_T" << endl;
        return -1;
    }

    // skip to the start of the program
    for (; itline != input.end(); ++itline)
    {
        if (itline->empty())
            continue;

        if (itline->front() == 'B')
        {
            ++itline;
            ++itline;
            ++itline;
        }
        else
            break;
    }

    // EXECUTE
    D16Regs regs;
    for (; itline != input.end(); ++itline)
    {
        D16Instr instr(*itline);
        auto fn = possibles[instr.op].front().second;
        regs = ((instr.*fn)(regs));
    }

    return regs.r[0];
}

// -------------------------------------------------------------------

inline char d18_read(uint32_t x, uint32_t y, const char* land, uint32_t w, uint32_t h)
{
    if (x >= w)
        return ' ';
    return land[x + y*w];
}

int day18(const stringlist& input, int64_t minutes)
{
    uint32_t w = (uint32_t)input.front().length();
    uint32_t h = (uint32_t)input.size();
    char* land = new char[w*h];
    char* row = land;
    for (auto& line : input)
    {
        copy(line.begin(), line.begin() + w, row);
        row += w;
    }

    // after 1000, assume it will have settled to a steady state and extrapolate the rest
    auto procminutes = min(minutes, 1000ll);
    vector<int> values;
    values.reserve(procminutes);

    int value = -1;
    char* flip = new char[w*h];
    for (int64_t minute = 0; minute < procminutes; ++minute)
    {
        char* pout = flip;

        for (uint32_t y = 0; y < h; ++y)
        {
            for (uint32_t x = 0; x < w; ++x)
            {
                char surrounds[8];
                char* surrend = surrounds + sizeof(surrounds);
                char* psurr = surrounds;

                if (y > 0)
                {
                    *(psurr++) = d18_read(x - 1, y - 1, land, w, h);
                    *(psurr++) = d18_read(x, y - 1, land, w, h);
                    *(psurr++) = d18_read(x + 1, y - 1, land, w, h);
                }
                *(psurr++) = d18_read(x - 1, y, land, w, h);
                *(psurr++) = d18_read(x + 1, y, land, w, h);
                if (y + 1 < h)
                {
                    *(psurr++) = d18_read(x - 1, y + 1, land, w, h);
                    *(psurr++) = d18_read(x, y + 1, land, w, h);
                    *(psurr++) = d18_read(x + 1, y + 1, land, w, h);
                }

                char curr = d18_read(x, y, land, w, h);
                *pout = curr;
                if (curr == '.')
                {
                    auto ntrees = count(surrounds, psurr, '|');
                    if (ntrees >= 3)
                    {
                        *pout = '|';
                    }
                }
                else if (curr == '|')
                {
                    auto nyards = count(surrounds, psurr, '#');
                    if (nyards >= 3)
                    {
                        *pout = '#';
                    }
                }
                else if (curr == '#')
                {
                    if ((find(surrounds, psurr, '|') == psurr) ||
                        (find(surrounds, psurr, '#') == psurr))
                    {
                        *pout = '.';
                    }
                }
                
                ++pout;
            }
        }

        swap(land, flip);

        auto ntrees = count(land, land + w*h, '|');
        auto nyards = count(land, land + w*h, '#');
        value = (int)(ntrees * nyards);
        values.push_back(value);
    }

    // if we're looking to know the result after a long time, find the steady state loop and extrapolate
    if (minutes > 1000)
    {
        // seek out repeated values
        int steady_loop = -1;
        for (int loop = 20; loop < procminutes - 1; ++loop)
        {
            auto ita = values.rbegin();
            auto itb = ita + loop;
            if (*ita == *itb)
            {
                bool foundloop = true;
                auto itend = itb;
                while (ita != itend)
                {
                    ++ita;
                    ++itb;
                    if (*ita != *itb)
                    {
                        foundloop = false;
                        break;
                    }
                }
                if (foundloop)
                {
                    if (*itend == *(itend + loop))
                    {
                        //cout << "*** GOT IT at l=" << loop << endl;
                        steady_loop = loop;
                        break;
                    }
                }
            }
        }

        _ASSERT(steady_loop > 0);
        int64_t remain = (minutes - procminutes) % steady_loop;
        value = *(values.rbegin() + (steady_loop - remain));
    }

    delete[] flip;
    delete[] land;
    return value;
}

// -------------------------------------------------------------------

int main()
{
    initcolours();
    srand((unsigned int)time(0));

    cout << GARLAND(2) << "  advent of code 2018  " << GARLAND(2) << endl;
 
    test(3, day1(READ("+1\n-2\n+3\n+1")));
    test(3, day1(READ("+1\n+1\n+1")));
    test(0, day1(READ("+1\n+1\n-2")));
    test(-6, day1(READ("-1\n-2\n-3")));
    gogogo(day1(LOAD(1)));

    test(0, day1_2(READ("+1\n-1")));
    test(10, day1_2(READ("+3\n+3\n+4\n-2\n-4")));
    test(5, day1_2(READ("-6\n+3\n+8\n+5\n-6")));
    test(14, day1_2(READ("+7\n+7\n-2\n-7\n-4")));
    gogogo(day1_2(LOAD(1)));

    test(12, day2(READ("abcdef\nbababc\nabbcde\nabcccd\naabcdd\nabcdee\nababab")));
    gogogo(day2(LOAD(2)));

    test<string>("fgij", day2_2(READ("abcde\nfghij\nklmno\npqrst\nfguij\naxcye\nwvxyz")));
    gogogo(day2_2(LOAD(2)));

    test(4, day3(READ("#1 @ 1,3: 4x4\n#2 @ 3, 1: 4x4\n#3 @ 5, 5: 2x2")));
    gogogo(day3(LOAD(3)));

    test(3, day3_2(READ("#1 @ 1,3: 4x4\n#2 @ 3, 1: 4x4\n#3 @ 5, 5: 2x2")));
    gogogo(day3_2(LOAD(3)));

    test(240, day4(LOAD(4t)));
    gogogo(day4(LOAD(4)));

    test(4455, day4_2(LOAD(4t)));
    gogogo(day4_2(LOAD(4)));

    test(0, day5(string("aA")));
    test(0, day5(string("abBA")));
    test(4, day5(string("abAB")));
    test(6, day5(string("aabAAB")));
    test(10, day5(string("dabAcCaCBAcCcaDA")));
    gogogo(day5(LOADSTR(5)));

    test(4, day5_2(string("dabAcCaCBAcCcaDA")));
    gogogo(day5_2(LOADSTR(5)));

    test(17, day6(READ("1, 1\n1, 6\n8, 3\n3, 4\n5, 5\n8, 9")));
    gogogo(day6(LOAD(6)));

    test(16, day6_2(READ("1, 1\n1, 6\n8, 3\n3, 4\n5, 5\n8, 9"), 32));
    gogogo(day6_2(LOAD(6), 10000));

    test<string>("CABDFE", day7(LOAD(7t), 6));
    gogogo(day7(LOAD(7), 26));

    test(15, day7_2(LOAD(7t), 6, 2, 0));
    gogogo(day7_2(LOAD(7), 26, 5, 60));

    test(138, day8(string("2 3 0 3 10 11 12 1 1 0 1 99 2 1 1 2")));
    gogogo(day8(LOADSTR(8)));

    test(66, day8_2(string("2 3 0 3 10 11 12 1 1 0 1 99 2 1 1 2")));
    gogogo(day8_2(LOADSTR(8)));

    test(32ll, day9(string("9 players; last marble is worth 25 points")));
    test(8317ll, day9(string("10 players; last marble is worth 1618 points")));
    test(146373ll, day9(string("13 players; last marble is worth 7999 points")));
    test(2764ll, day9(string("17 players; last marble is worth 1104 points")));
    test(54718ll, day9(string("21 players; last marble is worth 6111 points")));
    test(37305ll, day9(string("30 players; last marble is worth 5807 points")));
    gogogo(day9(string("466 players; last marble is worth 71436 points")));

    nononoD(day9(string("466 players; last marble is worth 7143600 points")));

    skip("because it needs fancy graphics");
    skip("again with graphics");

    test(-5, d11_calcpower(57, 122, 79));
    test(0, d11_calcpower(39, 217, 196));
    test(4, d11_calcpower(71, 101, 153));
    test(D11Point{ 33,45,3 }, day11(18));
    test(D11Point{ 21,61,3 }, day11(42));
    gogogo(day11(4455));
    
    nest(D11Point{ 90,269,16 }, day11_2(18));
    nest(D11Point{ 232,251,12 }, day11_2(42));
    nonono(day11_2(4455));

    test(325, day12(LOAD(12t), 20));
    gogogo(day12(LOAD(12), 20));
    gogogo(day12_2(LOAD(12)));

    test(D13Cart(7, 3), day13(LOAD(13t)));
    gogogo(day13(LOAD(13)));

    test(D13Cart(6, 4), day13_2(LOAD(13t2)));
    gogogo(day13_2(LOAD(13)));

    test<string>("5158916779", day14(9));
    test<string>("0124515891", day14(5));
    test<string>("9251071085", day14(18));
    test<string>("5941429882", day14(2018));
    gogogo(day14(652601));
    
    test(9, day14_2("51589"));
    test(5, day14_2("01245"));
    test(18, day14_2("92510"));
    test(2018, day14_2("59414"));
    nononoD(day14_2("652601"));

    test(27730, day15(LOAD(15t3)));
    test(36334, day15(LOAD(15t2)));
    test(39514, day15(LOAD(15t)));
    test(27755, day15(LOAD(15t4)));
    test(28944, day15(LOAD(15t5)));
    test(18740, day15(LOAD(15t6)));
    nononoD(day15(LOAD(15)));

    test(4988, day15_2(LOAD(15t3)));
    test(31284, day15_2(LOAD(15t)));
    test(3478, day15_2(LOAD(15t4)));
    test(6474, day15_2(LOAD(15t5)));
    test(1140, day15_2(LOAD(15t6)));
    nonono(day15_2(LOAD(15)));

    test(1, day16(READ("Before: [3, 2, 1, 1]\n9 2 1 2\nAfter:  [3, 2, 2, 1]\n\n\n")));
    gogogo(day16(LOAD(16)));

    gogogo(day16_2(LOAD(16)));

    skip("because it needs fancy graphics");
    skip("actually it didn't, but i like doing them");
    
    //jumptoday(18);

    test(1147, day18(LOAD(18t), 10));
    gogogo(day18(LOAD(18), 10));
    gogogo(day18(LOAD(18), 1000*1000*1000));

    // animate snow falling behind the characters in the console until someone presses a key
    return twinkleforever();
}

