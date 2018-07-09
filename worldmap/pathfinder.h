#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "world.h"
#include "world_global.h"
#include <QString>
#include <QFile>
#include <string>
#include <vector>
#include <iostream>
#include <tuple>
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


class Test
{
public:
    Test(int startx,int starty,int goalx, int goaly,float scaleg,float scaleh,
         std::shared_ptr<World> worldRead,std::vector<std::shared_ptr<Tile>>tileRead);

    float cost(int from_nodex,int from_nodey, int to_nodex,int to_nodey) ;

    template<typename Graph>
    void a_star_search
    (const Graph& graph,
     typename Graph::Location start,
     typename Graph::Location goal,
     std::unordered_map<typename Graph::Location, typename Graph::Location>& came_from,
     std::unordered_map<typename Graph::Location, float>& cost_so_far);

    template<typename Location>
    std::vector<Tile *>reconstruct_path(
        Location start,
        Location goal,
        std::unordered_map<Location, Location>& came_from
    );

    std::vector<Tile *> getPathResult() const
    {
        return pathResult;
    }

    void setStart(int startx,int starty)
    {
        startX = startx;
        startY = starty;
    }
    void setGoal(int goalx, int goaly)
    {
        goalX = goalx;
        goalY = goaly;
    }

private:
    std::vector<std::shared_ptr<Tile>> mapTiles;
    std::shared_ptr<World> world;
    std::vector<Tile*> pathResult;
    int startX;
    int startY;
    int goalX;
    int goalY;
    float new_cost;
    float scaleG;
    float scaleH;
};

#endif // PATHFINDER_H
