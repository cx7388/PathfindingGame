#ifndef TEST_H
#define TEST_H

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
    Test(int startx,int starty,int goalx, int goaly);

//    std::vector<std::shared_ptr<Tile>> untoshare();

    std::vector<Tile*> getPathResult() const;
    void setStart(int x,int y);
    void setGoal(int x,int y);

    double cost(int from_nodex,int from_nodey, int to_nodex,int to_nodey) ;

    template<typename Graph>
    void a_star_search
    (const Graph& graph,
     typename Graph::Location start,
     typename Graph::Location goal,
     std::unordered_map<typename Graph::Location, typename Graph::Location>& came_from,
     std::unordered_map<typename Graph::Location, double>& cost_so_far);

    template<typename Location>
    std::vector<Tile *>reconstruct_path(
        Location start,
        Location goal,
        std::unordered_map<Location, Location>& came_from
    );

private:
    std::vector<std::unique_ptr<Tile>> mapTiles;
    World *world;
    std::vector<Tile*> pathResult;
    int startX;
    int startY;
    int goalX;
    int goalY;

};

#endif // TEST_H
