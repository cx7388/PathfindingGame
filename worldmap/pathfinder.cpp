#include "pathfinder.h"
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

#include "pathfinder.h"
#include "world.h"
#include "world_global.h"
#include "global_config.h"

namespace std
{
template <>
struct hash<std::tuple<int, int> > {
    inline size_t operator()(const std::tuple<int, int>& location) const
    {
        int x, y;
        std::tie(x, y) = location;
        return x * 1812433253 + y;
    }
};
}

struct SquareGrid {

    typedef std::tuple<int, int> Location;
    static std::array<Location, 4> DIRS;

    int width, height;//width and height of the whole map
    std::unordered_set<Location> walls;//location of walls

    SquareGrid(int width_, int height_)
        : width(width_), height(height_) {}

    inline bool passable(Location id) const  //
    {
        return !walls.count(id);
    }

    inline bool in_bounds(Location id) const
    {
        int x, y;
        std::tie(x, y) = id;
        return 0 <= x && x < width && 0 <= y && y < height;
    }

    std::vector<Location> neighbors(Location id) const
    {
        int x, y, dx, dy;
        std::tie(x, y) = id;
        std::vector<Location> results;

        for (auto dir : DIRS) {
            std::tie(dx, dy) = dir;
            Location next(x + dx, y + dy);
            if (in_bounds(next) && passable(next)) {
                results.push_back(next);
            }
        }

        if ((x + y) % 2 == 0) {
            std::reverse(results.begin(), results.end());
        }

        return results;
    }
};

template<typename T, typename priority_t>
struct PriorityQueue {
    typedef std::pair<priority_t, T> PQElement;
    std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> elements;

    inline void put(T item, priority_t priority)
    {
        elements.emplace(priority, item);
    }

    inline bool empty() const
    {
        return elements.empty();
    }

    inline T get()
    {
        T best_item = elements.top().second;
        elements.pop();
        return best_item;
    }
};


struct GridWithWeights : SquareGrid {
    GridWithWeights(int w, int h) : SquareGrid(w, h) {}
};

std::array<SquareGrid::Location, 4> SquareGrid::DIRS{ Location{ 1, 0 }, Location{ 0, -1 }, Location{ -1, 0 }, Location{ 0, 1 },
//                                                    Location{ 1, 1 }, Location{ 1, -1 }, Location{ -1, 1 }, Location{ -1, -1 }
                                                    };

void add_rect(SquareGrid& grid, int x1, int y1, int x2, int y2)
{
    for (int x = x1; x < x2; ++x) {
        for (int y = y1; y < y2; ++y) {
            grid.walls.insert(SquareGrid::Location{ x, y });
        }
    }
}

GridWithWeights make_diagram(int startx,int starty,std::shared_ptr<World> world,std::vector<std::shared_ptr<Tile>>mapTiles)
{

    GridWithWeights grid(world->getCols(), world->getRows());
    std::cout<<"previous maptile value is"<< mapTiles[starty*(world->getCols()) + startx] ->getValue() <<std::endl;

    std::cout<<"maptile value is"<< mapTiles[starty*(world->getCols()) + startx] ->getValue() <<std::endl;
    for(auto b = mapTiles.begin()+1; b<mapTiles.end(); b++) {
        if(std::isinf((*b)->getValue()))
            add_rect(grid, (*b)->getXPos(),(*b)->getYPos(),(*b)->getXPos()+1, (*b)->getYPos()+1);
        //std::cout << b->getXPos() << " " << b->getYPos() <<std::endl;
    }
    return grid;

}


float Test::cost(int from_nodex,int from_nodey, int to_nodex,int to_nodey)
{
    float costa, costb,cost;

    costa = mapTiles[from_nodey*(world->getCols()) +from_nodex]->getValue();
    costb = mapTiles[to_nodey*(world->getCols()) +to_nodex]->getValue();

    if(std::isinf(costa) || std::isinf(costb) ) {
        cost = 2;
    } else {
        cost = abs(costa-costb)+1;
    }
    return cost;
}

template<typename Location>
std::vector<Tile *> Test::reconstruct_path(
    Location start,
    Location goal,
    std::unordered_map<Location, Location>& came_from
)
{
    std::vector<Tile *> path;
    Location current = goal;
    Tile *tilenew = new Tile(0,0,0);
    float costa = 0;

    costa = mapTiles[std::get<1>(current)*(world->getCols()) +std::get<0>(current)]->getValue();

    tilenew->setValue(costa);
    tilenew->setXPos(std::get<0>(current));
    tilenew->setYPos(std::get<1>(current));
    path.push_back(tilenew);

    while (current != start) {
        current = came_from[current];

        Tile *tilenew = new Tile(0,0,0);
        float costa = 0;

        costa = mapTiles[std::get<0>(current)*(world->getCols()) +std::get<1>(current)]->getValue();

        tilenew->setValue(costa);
        tilenew->setXPos(std::get<0>(current));
        tilenew->setYPos(std::get<1>(current));

        path.push_back(tilenew);
    }
    std::reverse(path.begin(), path.end());
    return path;
}

inline float heuristic(SquareGrid::Location a, SquareGrid::Location b)
{
    int x1, y1, x2, y2;
    std::tie(x1, y1) = a;
    std::tie(x2, y2) = b;
    return abs(x1 - x2) + abs(y1 - y2);
}

template<typename Graph>
void Test::a_star_search
(const Graph& graph,
 typename Graph::Location start,
 typename Graph::Location goal,
 std::unordered_map<typename Graph::Location, typename Graph::Location>& came_from,
 std::unordered_map<typename Graph::Location, float>& cost_so_far)
{
    typedef typename Graph::Location Location;
    PriorityQueue<Location, float> frontier;
    frontier.put(start, 0);

    came_from[start] = start;
    cost_so_far[start] = 0;

    while (!frontier.empty()) {
        auto current = frontier.get();

        if (current == goal) {
            break;
        }

        for (auto next : graph.neighbors(current)) {
            new_cost = cost_so_far[current] + cost(std::get<0>(current), std::get<1>(current),std::get<0>(next), std::get<1>(next));
            if (!cost_so_far.count(next) || new_cost < cost_so_far[next]) {
                cost_so_far[next] = new_cost;
                float priority = scaleG*new_cost + scaleH*heuristic(next, goal);
                frontier.put(next, priority);
                came_from[next] = current;
            }
        }
    }
}

Test::Test(int startx,int starty,int goalx, int goaly,float scaleg,float scaleh,
           std::shared_ptr<World> worldRead,std::vector<std::shared_ptr<Tile>>tileRead):
    startX{startx},startY{starty},goalX{goalx},goalY{goaly},scaleG{scaleg},scaleH{scaleh},
    world{worldRead},mapTiles{tileRead}
{
    clock_t starts,ends;
    GridWithWeights grid = make_diagram(startx,starty,worldRead,tileRead);
    SquareGrid::Location start{startX,startY};//33,33
    SquareGrid::Location goal{goalX,goalY};//1174,1198
    std::unordered_map<SquareGrid::Location, SquareGrid::Location> came_from;
    std::unordered_map<SquareGrid::Location, float> cost_so_far;
    std::cout << "start " << std::endl;
    starts = clock();
    a_star_search(grid, start, goal, came_from, cost_so_far);
    ends = clock();
    std::cout << "finish searching " << std::endl;
    std::cout<<ends-starts <<" us"<<std::endl;
    pathResult = reconstruct_path(start, goal, came_from);
}



