
#include "day15.h"

#include <algorithm>
#include<iterator>
#include <vector>


class Battle;

class theyre_all_dead_sir : public exception
{

};


struct Point
{
    uint8_t x, y;

    Point() {/**/}
    Point(uint32_t x, uint32_t y)
    {
        _ASSERT(x <= 0xff && y <= 0xff);
        this->x = (uint8_t)x;
        this->y = (uint8_t)y;
    }

    bool operator==(const Point& o) const
    {
        return x == o.x && y == o.y;
    }
    bool operator!=(const Point& o) const
    {
        return x != o.x || y != o.y;
    }
    bool operator<(const Point& o) const
    {
        if (y < o.y) return true;
        if (y == o.y && x < o.x) return true;
        return false;
    }

    Point up() const    { return Point(x, y - 1); }
    Point down() const  { return Point(x, y + 1); }
    Point left() const  { return Point(x - 1, y); }
    Point right() const { return Point(x + 1, y); }
};


struct Path
{
    Point to;
    vector<Point> steps;
};


class Actor
{
    Point pos;
    char race;
    int hp;
    int ap;


    static void addopen(vector<Point>& open, uint8_t x, uint8_t y, const Battle& battle);

    bool trypathfind(const Point& dest, const Battle& battle, Path& outpath) const;

public:
    Actor() {/**/}
    Actor(uint8_t x, uint8_t y, char race, int ap) : pos(x,y), race(race), ap(ap)
    {
        hp = 200;
    }

    bool operator<(const Actor& o) const
    {
        return pos < o.pos;
    }

    bool iself() const { return race == 'E'; }
    bool isalive() const { return hp > 0; }
    const Point& getpos() const { return pos; }
    int gethp() const { return hp; }

    bool canattack(const Battle & battle) const;
    void move(Battle& battle);
    void attack(Battle& battle);

    friend ostream& operator<<(ostream& os, const Battle& battle);
};


class Battle
{
    uint32_t width, height;
    char* map;
    vector<Actor> actors;
    int ndeadelves;

    void sort_actors();
    void clean_up_the_dead();

public:
    Battle(const stringlist& input, int elfpower);
    ~Battle()
    {
        delete[] map;
    }

    bool tick();

    char get_tile(const Point& p) const
    {
        return get_tile(p.x, p.y);
    }
    char get_tile(uint32_t x, uint32_t y) const
    {
        if (x >= width || y >= height)
            return '#';

        return map[x + y*width];
    }
    void set_tile(const Point& p, char c)
    {
        set_tile(p.x, p.y, c);
    }
    void set_tile(uint32_t x, uint32_t y, char c)
    {
        _ASSERT(get_tile(x, y) == '.');

        if (x >= width || y >= height)
            return;

        map[x + y * width] = c;
    }
    void clear_tile(const Point& p)
    {
        _ASSERT(get_tile(p) != '.');

        if (p.x >= width || p.y >= height)
            return;

        map[p.x + p.y * width] = '.';
    }

    int getscore() const
    {
        int score = 0;
        for (auto& actor : actors)
        {
            if (actor.isalive())
                score += actor.gethp();
        }
        return score;
    }

    bool gameover() const;
    bool deadelves() const { return ndeadelves > 0; }

    friend ostream& operator<<(ostream& os, const Battle& battle);
    friend class Actor;
    friend class Dijkstra;
};


Battle::Battle(const stringlist& input, int elfpower): ndeadelves(0)
{
    height = input.size();
    width = 0;
    for (auto& line : input)
        width = max(width, line.length());

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

            if (c == 'G' || c == 'E')
                actors.emplace_back(x, y, c, (c=='E')?elfpower:3);
        }
        ++y;
    }
}


// returns true if one side has won before this round completed
bool Battle::tick()
{
    try
    {
        sort_actors();

        for (auto& actor : actors)
        {
            if (!actor.isalive())
                continue;

            if (!actor.canattack(*this))
                actor.move(*this);

            if (actor.canattack(*this))
                actor.attack(*this);
        }

        clean_up_the_dead();
    }
    catch(theyre_all_dead_sir&)
    {
        clean_up_the_dead();
        return true;
    }

    return false;
}

void Battle::sort_actors()
{
    sort(actors.begin(), actors.end());
}

void Battle::clean_up_the_dead()
{
    for (const Actor& a : actors)
    {
        if (!a.isalive() && a.iself())
            ++ndeadelves;
    }

    actors.erase(remove_if(actors.begin(), actors.end(), [](const Actor& a)->bool {
        return !a.isalive();
    }), actors.end());
}


bool Battle::gameover() const
{
    size_t nelves = 0;
    size_t nglobs = 0;
    for (auto& actor : actors)
    {
        if (actor.isalive())
        {
            if (actor.iself())
                ++nelves;
            else
                ++nglobs;
        }
    }

    return (nelves == 0) || (nglobs == 0);
}


void Actor::addopen(vector<Point>& open, uint8_t x, uint8_t y, const Battle& battle)
{
    // FIXME?: all these finds would be faster if open was sorted...
    for (int xoffs = -1; xoffs <= 1; xoffs += 2)
    {
        Point p(x + xoffs, y);
        if (battle.get_tile(p.x, p.y) == '.')
        {
            if( find(open.begin(), open.end(), p) == open.end() )
                open.push_back(p);
        }
    }
    for (int yoffs = -1; yoffs <= 1; yoffs += 2)
    {
        Point p(x, y + yoffs);
        if (battle.get_tile(p.x, p.y) == '.')
        {
            if (find(open.begin(), open.end(), p) == open.end())
                open.push_back(p);
        }
    }
}


class Dijkstra
{
    const Battle& battle;
    uint32_t width;
    uint32_t height;
    Point from;
    Point to;

    // top bit = unvisited, bottom 15 = distance
    uint16_t* cost;

    vector<Point> open;

    void visit(const Point& p);

    bool isvisited(const Point& p) const
    {
        if (p.x >= width || p.y >= height)
            return true;

        return (cost[p.x + p.y*width] & 0x8000) == 0;
    }

    bool istraversable(const Point& p) const
    {
        return battle.map[p.x + p.y*width] == '.';
    }

    uint32_t gettcost(const Point& p) const
    {
        if (p.x >= width || p.y >= height)
            return 0x7fff;

        return cost[p.x + p.y*width] & 0x7fff;
    }
    void settcost(const Point& p, uint16_t newtcost)
    {
        _ASSERT(newtcost <= 0x7fff);

        if (p.x >= width || p.y >= height)
            return;

        auto tcost = cost[p.x + p.y*width];
        cost[p.x + p.y*width] = newtcost | (tcost & 0x8000);
    }
    void marktcost(const Point& p, uint16_t neighbortcost)
    {
        _ASSERT(neighbortcost <= 0x7fff);

        if (p.x >= width || p.y >= height)
            return;

        auto oldtcost = cost[p.x + p.y*width];
        auto tcost = min(oldtcost, neighbortcost + 1);
        cost[p.x + p.y*width] = tcost | (oldtcost & 0x8000);
    }

    void visitneighbour(const Point& p, uint16_t tcost)
    {
        if (!isvisited(p) && istraversable(p))
        {
            marktcost(p, tcost);

            if( find( open.begin(), open.end(), p ) == open.end() )
                open.push_back(p);
        }
    }

    void buildpath(Path& outpath) const;

public:

    Dijkstra(const Battle& battle, const Point& from, const Point& to)
        : battle(battle)
        , from(from)
        , to(to)
    {
        width = battle.width;
        height = battle.height;
        cost = new uint16_t[width * height];
        fill(cost, cost + width * height, 0xffff);
        cost[from.x + from.y*width] = 0;

        open.reserve(50);
    }
    ~Dijkstra()
    {
        delete[] cost;
    }

    bool go(Path& outpath);
};


void Dijkstra::visit(const Point& p)
{
    _ASSERT(istraversable(p) || p == from);
    auto tcost = gettcost(p);

    visitneighbour(p.up(), tcost);
    visitneighbour(p.left(), tcost);
    visitneighbour(p.right(), tcost);
    visitneighbour(p.down(), tcost);

    // mark as visited
    cost[p.x + p.y*width] &= 0x7fff;
}

void Dijkstra::buildpath(Path& outpath) const
{
    outpath.to = to;

    // follow path of least tcost back from 'to' to 'from'. <-- lol
    auto nsteps = gettcost(to);
    auto& steps = outpath.steps;
    steps.reserve(nsteps);

    Point curr = to;
    while (curr != from)
    {
        steps.push_back(curr);

        auto next = curr.up();
        auto mintcost = gettcost(next);
        // ugh there must be a better way to express this :/
        if (gettcost(curr.left()) < mintcost)
        {
            next = curr.left();
            mintcost = gettcost(curr.left());
        }
        if (gettcost(curr.right()) < mintcost)
        {
            next = curr.right();
            mintcost = gettcost(curr.right());
        }
        if (gettcost(curr.down()) < mintcost)
        {
            next = curr.down();
            mintcost = gettcost(curr.down());
        }

        curr = next;
    }

    reverse(steps.begin(), steps.end());
}

bool Dijkstra::go(Path& outpath)
{
    // THINKS: open could be sorted list<> of Point,tcost pairs?
    Point curr = from;
    open.push_back(curr);
    while (!open.empty())
    {
        // find open space with minimum tentative cost
        curr = open.front();
        auto mintcost = gettcost(curr);
        for (auto it = open.begin() + 1; it != open.end(); ++it)
        {
            auto tcost = gettcost(*it);
            if (tcost < mintcost)
            {
                curr = *it;
                mintcost = tcost;
            }
        }

        // if it's our destination, then we're done marking costs
        if (curr == to)
        {
            buildpath(outpath);
            return true;
        }

        // visit it!
        visit(curr);
        open.erase(find(open.begin(), open.end(), curr));
    }

    return false;
}


bool Actor::trypathfind(const Point& dest, const Battle& battle, Path& outpath) const
{
    Dijkstra dijkstra(battle, pos, dest);
    return dijkstra.go(outpath);
}


bool Actor::canattack(const Battle& battle) const
{
    if (hp <= 0)
        return false;

    char enemyrace = (race == 'E') ? 'G' : 'E';
    if ((battle.get_tile(pos.up()) == enemyrace) ||
        (battle.get_tile(pos.left()) == enemyrace) ||
        (battle.get_tile(pos.right()) == enemyrace) ||
        (battle.get_tile(pos.down()) == enemyrace))
        return true;

    return false;
}


void Actor::move(Battle& battle)
{
    // step 1: find all non-wall spaces within one space of the enemy
    vector<Point> inrange;
    inrange.reserve(battle.actors.size() * 4);
    bool enemies = false;
    for (auto& them : battle.actors)
    {
        if (them.race == race)
            continue;
        if (!them.isalive())
            continue;

        enemies = true;
        addopen(inrange, them.pos.x, them.pos.y, battle);
    }

    if (!enemies)
        throw theyre_all_dead_sir();

    sort(inrange.begin(), inrange.end());

    // step 2: pathfind to each
    vector<Path> paths;
    for (auto& dest : inrange)
    {
        Path path;
        if (trypathfind(dest, battle, path))
        {
            paths.push_back(path);
        }
    }

    // if we have nowhere to go, just stop here
    if (paths.empty())
        return;

    // step 3: discard all but the shortest path(s)
    auto itshortest = min_element(paths.begin(), paths.end(), [](const Path& a, const Path& b)->bool {
        return a.steps.size() < b.steps.size();
    });
    paths.erase(remove_if(paths.begin(), paths.end(), [&](const Path& p)->bool {
        return p.steps.size() > itshortest->steps.size();
    }), paths.end());

    // step 4: choose the remaining destination first in reading order
    auto itchoice = min_element(paths.begin(), paths.end(), [](const Path& a, const Path& b)->bool {
        return a.to < b.to;
    });

    // step 5: MOVE!
    battle.clear_tile(pos);
    pos = itchoice->steps.front();
    battle.set_tile(pos, race);
}


void Actor::attack(Battle& battle)
{
    // step 1: find all living enemies within range
    vector<Actor*> attackable;
    bool enemies = false;
    for (auto& other : battle.actors)
    {
        if (other.race == race)
            continue;
        if (other.hp <= 0)
            continue;

        enemies = true;

        if (other.pos.x == pos.x)
        {
            if (pos.y - 1 == other.pos.y || pos.y + 1 == other.pos.y)
                attackable.push_back(&other);
        }
        else if (other.pos.y == pos.y)
        {
            if (pos.x - 1 == other.pos.x || pos.x + 1 == other.pos.x)
                attackable.push_back(&other);
        }
    }

    if (!enemies)
        throw theyre_all_dead_sir();
    if (attackable.empty())
        return;

    // step 2: choose one with lowest hp
    auto itdeadest = min_element(attackable.begin(), attackable.end(), [](const Actor* a, const Actor* b)->bool {
        return a->hp < b->hp;
    });
    attackable.erase(remove_if(attackable.begin(), attackable.end(), [&](const Actor* a)->bool {
        return a->hp > (*itdeadest)->hp;
    }), attackable.end());

    // step 3: break any stalemate using reading order
    sort(attackable.begin(), attackable.end(), [](const Actor* a, const Actor* b)->bool {
        return *a < *b;
    });

    Actor* ptarget = attackable.front();
    _ASSERT(ptarget->hp > 0);
    _ASSERT(ptarget->race != race);
    ptarget->hp -= ap;
    if (!ptarget->isalive())
    {
        battle.clear_tile(ptarget->pos);

        if (this != &battle.actors.back() && battle.gameover())
            throw theyre_all_dead_sir();
    }
}




ostream& operator<<(ostream& os, const Battle& battle)
{
    for (uint32_t y = 0; y < battle.height; ++y)
    {
        os.write(battle.map + y*battle.width, battle.width);

        vector<Actor> acs;
        insert_iterator<vector<Actor>> itin(acs, acs.begin());
        copy_if(battle.actors.begin(), battle.actors.end(), itin, [=](auto& a)->bool {
            return (a.pos.y == y);
        });

        if (!acs.empty())
        {
            sort(acs.begin(), acs.end(), [](auto& a, auto& b)->bool {
                return a.pos.x < b.getpos().x;
            });

            os << "   ";

            for (auto& actor : acs)
            {
                os << actor.race << '(' << actor.hp << ")  ";
            }
        }

        os << '\n';
    }
    return os;
}



int day15(const stringlist& input)
{
    Battle battle(input, 3);

    int i;
    for (i = 1; i < 200; ++i)
    {
        //cout << "Before Round " << i << ":\n" << battle << endl;

        if (battle.tick())
        {
            //cout << "Finished after " << i-1 << " complete rounds\n";
            //cout << battle << endl;
            //cout << "                     hp score is " << battle.getscore() << endl;

            return (i-1) * battle.getscore();
        }
    }

    return -1;
}

int day15_2(const stringlist& input)
{
    for (int elfpower = 3; ; ++elfpower)
    {
        Battle battle(input, elfpower);

        int i;
        for (i = 1; i < 200; ++i)
        {
            //cout << "Before Round " << i << ":\n" << battle << endl;

            bool gameover = battle.tick();

            if (battle.deadelves())
                break;

            if (gameover)
            {
                //cout << "Finished after " << i - 1 << " complete rounds\n";
                //cout << battle << endl;
                //cout << "                     hp score is " << battle.getscore() << ", ELF POWER is " << elfpower << endl;

                return (i - 1) * battle.getscore();
            }
        }
    }

    return -1;
}