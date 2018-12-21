#include <vector>
#include <conio.h>

#include "../day10/pixie/pixie.h"

#include "../adventofcode2018/harness.h"


int screenw = 1000;
int screenh = 880;


Pixie::Window window;


struct D17Span
{
    uint32_t minx, maxx;    // ranges are inclusive
    uint32_t miny, maxy;

    D17Span() {/**/ }
    D17Span(uint32_t minx, uint32_t maxx, uint32_t miny, uint32_t maxy)
        : minx(minx), maxx(maxx), miny(miny), maxy(maxy)
    {/**/
    }

    D17Span(const string& line)
    {
        // y=7, x=495..501
        istringstream is(line);
        char dummy;
        if (line[0] == 'x')
        {
            is.ignore(1000, '=');
            is >> maxx;
            minx = maxx;

            is.ignore(1000, '=');
            is >> miny >> dummy >> dummy >> maxy;
        }
        else
        {
            is.ignore(1000, '=');
            is >> maxy;
            miny = maxy;

            is.ignore(1000, '=');
            is >> minx >> dummy >> dummy >> maxx;
        }
    }
};

inline void d17_fill(char* map, uint32_t xoff, uint32_t yoff, uint32_t width, const D17Span& clay)
{
    D17Span local = clay;
    local.minx -= xoff;
    local.maxx -= xoff;
    local.miny -= yoff;
    local.maxy -= yoff;

    for (uint32_t y = local.miny; y <= local.maxy; ++y)
    {
        char* prow = map + y * width;
        memset(prow + local.minx, '#', 1 + local.maxx - local.minx);
    }
}

int day17(const stringlist& input)
{
    // load the data
    vector<D17Span> clays;
    clays.reserve(input.size());
    D17Span space(~0u, 0, ~0u, 0);
    for (auto& line : input)
    {
        clays.emplace_back(line);

        auto& s = clays.back();
        space.minx = min(space.minx, s.minx);
        space.maxx = max(space.maxx, s.maxx);
        space.miny = min(space.miny, s.miny);
        space.maxy = max(space.maxy, s.maxy);
    }

    // embiggen our space to catch edge drips
    space.minx--;
    space.maxx++;

    // set up our space
    uint32_t width = 1 + space.maxx - space.minx;
    uint32_t height = 1 + space.maxy - space.miny;
    char* map = new char[width * height];
    memset(map, '.', width * height);

    screenw = width;
    screenh = height;
    if (!window.Open(L"day17 guest appearance in a window!", screenw, screenh, false))
        exit(1);

    uint32_t xoff = space.minx;
    uint32_t yoff = space.miny;

    // fill in the clay
    for (auto& clay : clays)
    {
        d17_fill(map, xoff, yoff, width, clay);
    }

    // wet the sand beneath the spring
    map[500 - xoff] = '|';
    bool wetted;
    int nwetted = 1;
    bool printed = false;
    do {
        //if (!window.IsKeyDown(Pixie::Key_Space))
        //{
        //    Sleep(100);
        //    if (!window.Update())
        //        break;
        //    wetted = true; 
        //    continue;
        //}

        wetted = false;

        // go from the bottom up, omitting the top line
        // FIXME: would be a lot quicker if we just remembered the 3-4 lines that could possibly need updating...
        for (uint32_t ly = height - 1; ly > 0; --ly)
        {
            // propagate each spot of moisture in the previous row
            auto abovebegin = map + ((ly - 1) * width);
            auto aboveend = abovebegin + width;
            for (auto prev = abovebegin; prev != aboveend; ++prev)
            {
                char prevc = *prev;
                if (prevc == '|')
                {
                    // propagate l&r as long as there's clay or water below
                    if (ly != height - 1)
                    {
                        auto currbegin = aboveend;
                        auto currend = currbegin + width;

                        // wind to the left
                        int nwound = 0;
                        auto curr = prev + width;
                        while (curr != currbegin)
                        {
                            auto below = curr + width;
                            if ((*below == '#' || *below == '~') &&
                                (*(curr - 1) != '#'))
                            {
                                --curr;
                                nwound++;
                            }
                            else
                            {
                                break;
                            }
                        }

                        // fill to the right as long as we can
                        while (curr != currend)
                        {
                            if (*curr == '.')
                            {
                                *curr = '|';
                                wetted = true;
                                ++nwetted;
                            }

                            auto below = curr + width;
                            if ((nwound <= 0) &&       // deal with flows off to the left
                                ((*below != '#' && *below != '~') ||
                                (*(curr + 1) == '#')))
                            {
                                break;
                            }
                            else
                            {
                                ++curr;
                                --nwound;
                            }
                        }
                    }
                    else
                    {
                        auto pcurr = prev + width;
                        if (*pcurr != '|')
                        {
                            *pcurr = '|';
                            wetted = true;
                            ++nwetted;
                        }
                    }
                }
            }
        }

        // now the water's got everywhere by flowing, find where it's settled

        // go from the bottom up, omitting the top & bottom lines
        for (uint32_t ly = height - 2; ly > 0; --ly)
        {
            auto currbegin = map + (ly*width);
            auto currend = currbegin + width;

            auto curr = currbegin;
            while (curr != currend)
            {
                // skip to the next clay
                curr = find(curr, currend, '#');
                if (curr == currend || (curr + 1 == currend))
                    break;

                ++curr;

                if (*curr == '|')
                {
                    // find the span of contained water
                    auto start = curr;
                    bool contained = true;
                    while (*curr == '|')
                    {
                        auto below = curr + width;
                        if (*below != '#' &&* below != '~')
                        {
                            contained = false;
                            break;
                        }
                        ++curr;
                    }
                    if (*curr == '#' && contained)
                    {
                        fill(start, curr, '~');
                    }
                }
            }
        }

        // render
        uint32_t* pixbuf = window.GetPixels();
        char* curr = map;
        while (curr != map + (width * height))
        {
            char c = *curr;
            if (c == '.')
                *pixbuf = 0xbb8844;
            else if (c == '|')
                *pixbuf = 0xbbddff;
            else if (c == '~')
                *pixbuf = 0x5588ff;
            else
                *pixbuf = 0x886600;

            ++curr;
            ++pixbuf;
        }

        if (!window.Update())
            break;

        if (!wetted && !printed)
        {
            cout << GARLAND(4) << " result: " << nwetted << " " << GARLAND(4) << endl;
            cout << "\n\naquifers = " << count(map, map + width*height, '~') << endl;

            ofstream ofs("dump.txt");
            for (uint32_t y = 0; y < height; ++y)
            {
                ofs.write(map + y*width, width);
                ofs << '\n';
            }

            printed = true;
        }

    } while (!window.IsKeyDown(Pixie::Key::Key_Escape));

    delete[] map;
    return nwetted;
}


int main()
{

    // load the points
    auto input = stringlist::fromfile("day17.txt");
    cout << day17(input) << endl;

    //window.Close();
    Sleep(1000);
    return twinkleforever();
}