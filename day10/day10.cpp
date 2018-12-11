#include <vector>
#include <conio.h>

#include "pixie/pixie.h"

#include "../adventofcode2018/harness.h"


const int screenw = 500;
const int screenh = 440;
const int xrange = screenw/2;
const int yrange = screenh/2;

const int maxidleframes = 2000;


struct D10Point
{
    int px, py;
    int vx, vy;

    D10Point() {/**/}
    D10Point(const string& desc)
    {
        // position=< 1,  8> velocity=< 1, -1>
        istringstream is(desc);
        is.seekg(10, ios_base::cur);
        char comma;
        is >> skipws >> px >> comma >> py;
        is.seekg(12, ios_base::cur);
        is >> skipws >> vx >> comma >> vy;
    }

    bool tick()
    {
        px += vx;
        py += vy;

        return (abs(px) <= xrange) && (abs(py) <= yrange);
    }
};

inline void plot(uint32_t* pbackbuf, D10Point& p, uint32_t col=0xff7500)
{
    int x = p.px + xrange;
    int y = p.py + yrange;
    if (x < 0 || x >= screenw || y < 0 || y >= screenh)
        return;

    pbackbuf[y*screenw + x] = col;
}


int main()
{
    Pixie::Window window;
    if (!window.Open(L"day10 guest appearance in a window!", screenw, screenh, true))
        return 0;

    // load the points
    auto input = stringlist::fromfile("day10.txt");
    vector<D10Point> points;
    points.reserve(input.size());
    for (auto& line : input)
        points.emplace_back(line);

    bool evershown = false;
    int frame = 0;
   /* for (; frame < 11820; ++frame)
    {
        for (auto& point : points)
        {
            point.tick();
        }
    }*/
    while (!window.IsKeyDown(Pixie::Key_Escape))
    {
        if (window.HasKeyGoneDown(Pixie::Key_F1))
        {
            bool anyinrange = false;
            while (!anyinrange)
            {
                for (auto& point : points)
                {
                    if (point.tick())
                        anyinrange = true;
                }
                frame++;
            }
        }

        if (window.IsKeyDown(Pixie::Key_Space))
        {
            uint32_t* pixels = window.GetPixels();
            fill(pixels, pixels + screenw * screenh, 0);

            bool anyinrange = false;
            for (auto& point : points)
            {
                if (point.tick())
                {
                    anyinrange = true;
                    plot(pixels, point);
                }
            }

            frame++;
        }

        Sleep(100);
        if (!window.Update())
            break;
    }

    window.Close();
    return 0;
}