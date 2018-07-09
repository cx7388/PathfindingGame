#include "test.h"
#include <time.h>
#include <iostream>
#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <vector>
#include <utility>
#include <queue>
#include <tuple>
#include <algorithm>
#include <functional>
#include <cmath>

#include "world.h"
#include "world_global.h"


using namespace std;
using std::unordered_map;
using std::unordered_set;
using std::array;
using std::vector;
using std::queue;
using std::priority_queue;
using std::pair;
using std::tuple;
using std::tie;
using std::string;

// Helpers for SquareGrid::Location

// When using std::unordered_map<T>, we need to have std::hash<T> or
// provide a custom hash function in the constructor to unordered_map.
// Since I'm using std::unordered_map<tuple<int,int>> I'm defining the
// hash function here. It would be nice if C++ automatically provided
// the hash function for tuple and pair, like Python does. It would
// also be nice if C++ provided something like boost::hash_combine. In
// any case, here's a simple hash function that combines x and y:
namespace std
{
template <>
struct hash<tuple<int, int> > {
    inline size_t operator()(const tuple<int, int>& location) const
    {
        int x, y;
        tie(x, y) = location;
        return x * 1812433253 + y;
    }
};
}

struct SquareGrid {
    typedef tuple<int, int> Location;
    static array<Location, 4> DIRS;

    int width, height;//width and height of the whole map
    unordered_set<Location> walls;//location of walls

    SquareGrid(int width_, int height_)
        : width(width_), height(height_) {}

    inline bool in_bounds(Location id) const
    {
        int x, y;
        tie(x, y) = id;
        return 0 <= x && x < width && 0 <= y && y < height;
    }

    inline bool passable(Location id) const  //
    {
        return !walls.count(id);
    }

    vector<Location> neighbors(Location id) const
    {
        int x, y, dx, dy;
        tie(x, y) = id;
        vector<Location> results;

        for (auto dir : DIRS) {
            tie(dx, dy) = dir;
            Location next(x + dx, y + dy);
            if (in_bounds(next) && passable(next)) {
                results.push_back(next);
            }
        }

        if ((x + y) % 2 == 0) {
            // aesthetic improvement on square grids
            std::reverse(results.begin(), results.end());
        }

        return results;
    }
};


struct GridWithWeights : SquareGrid {
    //unordered_set<Location> forests;
    GridWithWeights(int w, int h) : SquareGrid(w, h) {}
//    double cost(Location from_node, Location to_node) const
//    {
//        for(auto a:mapTiles)
//        double from_node_value =
//    }
};

array<SquareGrid::Location, 4> SquareGrid::DIRS{ Location{ 1, 0 }, Location{ 0, -1 }, Location{ -1, 0 }, Location{ 0, 1 } };

void add_rect(SquareGrid& grid, int x1, int y1, int x2, int y2)
{
    for (int x = x1; x < x2; ++x) {
        for (int y = y1; y < y2; ++y) {
            grid.walls.insert(SquareGrid::Location{ x, y });
        }
    }
}

GridWithWeights make_diagram4()
{

    World world;
    std::vector<std::unique_ptr<Tile>> mapTiles;
    QString mapPath = "/home/mandi/Desktop/team307/assets/maze3.png";
    mapTiles = world.createWorld(mapPath);

    GridWithWeights grid(world.getCols(), world.getRows());

    for(auto &b :mapTiles) {
        if(std::isinf(b->getValue()))
            add_rect(grid, b->getXPos(),b->getYPos(), b->getXPos()+1, b->getYPos()+1);
        //std::cout << b->getXPos() << " " << b->getYPos() <<std::endl;
    }
    return grid;
}

double Test::cost(int from_nodex,int from_nodey, int to_nodex,int to_nodey)
{
    double costa, costb,cost;
    for(auto &a : mapTiles){
        if(a->getXPos()==from_nodex && a->getYPos() ==from_nodey){
            costa = a->getValue();
        }
    }
    for(auto &b : mapTiles){
        if(b->getXPos()==to_nodex && b->getYPos() ==to_nodey){
            costa = b->getValue();
        }
    }
    cost = abs(costa-costb);
    return cost;
}

template<typename T, typename priority_t>
struct PriorityQueue {
    typedef pair<priority_t, T> PQElement;
    priority_queue<PQElement, vector<PQElement>, std::greater<PQElement>> elements;

    inline bool empty() const
    {
        return elements.empty();
    }

    inline void put(T item, priority_t priority)
    {
        elements.emplace(priority, item);
    }

    inline T get()
    {
        T best_item = elements.top().second;
        elements.pop();
        return best_item;
    }
};


//template<typename Location>
//vector<Location> reconstruct_path(
//    Location start,
//    Location goal,
//    unordered_map<Location, Location>& came_from
//)
//{
//    vector<Location> path;
//    Location current = goal;
//    path.push_back(current);
//    while (current != start) {
//        current = came_from[current];
//        path.push_back(current);
//    }
//    return path;
//}


template<typename Location>
std::vector<Tile *> Test::reconstruct_path(
    Location start,
    Location goal,
    unordered_map<Location, Location>& came_from
)
{
    std::vector<Tile *> path;
    Location current = goal;
    Tile *tilenew = new Tile(0,0,0);
    float costa = 0;

    for(auto &a : mapTiles){
        if(a->getXPos()==std::get<0>(current) && a->getYPos() ==std::get<1>(current)){
            costa = a->getValue();
        }
    }

    tilenew->setValue(costa);
    tilenew->setXPos(std::get<0>(current));
    tilenew->setYPos(std::get<1>(current));
    path.push_back(tilenew);

    while (current != start) {
        current = came_from[current];

        Tile *tilenew = new Tile(0,0,0);
        float costa = 0;

        for(auto &a : mapTiles){
            if(a->getXPos()==std::get<0>(current) && a->getYPos() ==std::get<1>(current)){
                costa = a->getValue();
            }
        }

        tilenew->setValue(costa);
        tilenew->setXPos(std::get<0>(current));
        tilenew->setYPos(std::get<1>(current));

        path.push_back(tilenew);

        //std::cout << tilenew->getXPos() <<tilenew->getYPos() <<tilenew->getValue()<<std::endl;
    }

    //path.push_back(start); // optional
    //std::reverse(path.begin(), path.end());
    path.erase(path.begin());
    std::reverse(path.begin(), path.end());
    std::cout<< path[0]->getXPos() <<std::endl;
    return path;

//    Tile *tilenew = new Tile(0,0,0);
//    tilenew->setValue(0);
//    tilenew->setXPos(std::get<0>(id));
//    tilenew->setYPos(std::get<1>(id));
 //   pathVector.push_back(tilenew);
}

inline double heuristic(SquareGrid::Location a, SquareGrid::Location b)
{
    int x1, y1, x2, y2;
    tie(x1, y1) = a;
    tie(x2, y2) = b;
    return abs(x1 - x2) + abs(y1 - y2);
}

template<typename Graph>
void Test::a_star_search
(const Graph& graph,
 typename Graph::Location start,
 typename Graph::Location goal,
 unordered_map<typename Graph::Location, typename Graph::Location>& came_from,
 unordered_map<typename Graph::Location, double>& cost_so_far)
{
    typedef typename Graph::Location Location;
    PriorityQueue<Location, double> frontier;
    frontier.put(start, 0);

    came_from[start] = start;
    cost_so_far[start] = 0;

    while (!frontier.empty()) {
        auto current = frontier.get();

        if (current == goal) {
            break;
        }

        for (auto next : graph.neighbors(current)) {
            double new_cost = cost_so_far[current] + cost(std::get<0>(current), std::get<1>(current),std::get<0>(next), std::get<1>(next));
            if (!cost_so_far.count(next) || new_cost < cost_so_far[next]) {
                cost_so_far[next] = new_cost;
                double priority = new_cost + heuristic(next, goal);
                frontier.put(next, priority);
                came_from[next] = current;
            }
        }
    }
}

// This outputs a grid. Draw the path.
template<class Graph>

std::vector<Tile*> draw_grid(const Graph& graph,
                             vector<typename Graph::Location>* path = nullptr)
{
    // std::vector<std::unique_ptr<Tile>> pathVector;
    std::vector<Tile*> pathVector;


    for (int y = 0; y != graph.height; ++y) {
        for (int x = 0; x != graph.width; ++x) {
            typename Graph::Location id{ x, y };
            if (path != nullptr && find(path->begin(), path->end(), id) != path->end()) {
                //auto tilenew = std::unique_ptr<Tile>(new Tile(std::get<0>(id),std::get<1>(id),0));
                // pathVector.push_back(std::move(tilenew));
                Tile *tilenew = new Tile(0,0,0);
                tilenew->setValue(0);
                tilenew->setXPos(std::get<0>(id));
                tilenew->setYPos(std::get<1>(id));
                pathVector.push_back(tilenew);
                //std::cout<<tilenew->getXPos()<<std::endl;
                std::cout << id << std::endl;
            }
        }
    }
    return pathVector;
}


void Test::setStart(int startx,int starty)
{
    startX = startx;
    startY = starty;
}
void Test::setGoal(int goalx, int goaly)
{
    goalX = goalx;
    goalY = goaly;
}

Test::Test(int startx,int starty,int goalx, int goaly):
    startX{startx},startY{starty},goalX{goalx},goalY{goaly}
{
    clock_t starts,ends;
    GridWithWeights grid = make_diagram4();
    SquareGrid::Location start{startX,startY};//33,33
    SquareGrid::Location goal{goalX,goalY};//1174,1198
    unordered_map<SquareGrid::Location, SquareGrid::Location> came_from;
    unordered_map<SquareGrid::Location, double> cost_so_far;
    std::cout << "start " << std::endl;
    starts = clock();
    a_star_search(grid, start, goal, came_from, cost_so_far);
    ends = clock();
    std::cout << "finish searching " << std::endl;
    std::cout<<ends-starts<<std::endl;
// vector<SquareGrid::Location> path = reconstruct_path(start, goal, came_from);
    pathResult = reconstruct_path(start, goal, came_from);
    //pathResult=draw_grid(grid, &path);
    // std::cout << pathResult[0]->getXPos() << std::endl;
//    for(int i = 0; i<10; i++) {
//        int x = 0;
//        x=getPathResult()[i]->getXPos();
//        std::cout<<x<<std::endl;
//    }
}

std::vector<Tile *> Test::getPathResult() const
{
    return pathResult;
}



// For debugging it's useful to have an ostream::operator << so that we can write cout << foo
std::basic_iostream<char>::basic_ostream& operator<<(std::basic_iostream<char>::basic_ostream& out, tuple<int, int> loc)
{
    int x, y;
    tie(x, y) = loc;
    out << '(' << x << ',' << y << ')';
    return out;
}

