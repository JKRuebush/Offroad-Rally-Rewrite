#include <algorithm>
#include <cmath>
#include <map>
#include <queue>
#include <tuple>

#include "rallyAgent.h"

namespace {
// In order this tuple holds: the length of the shortest path found to the
// point, the previous point in the path, and the direction the previous point
// moved in to get to the current point.
typedef std::tuple<uint, Point, Direction> infoTuple;
}  // namespace

REGISTER_AGENT(Dijkstra)(MapInterface* const api) {
    std::map<Point, infoTuple> pointInfo;
    pointInfo.emplace(api->getStart(), infoTuple(0, Point(-1, -1), Direction::North));

    std::priority_queue<std::pair<uint, Point>, std::vector<std::pair<uint, Point>>,
                        std::greater<std::pair<uint, Point>>>
        frontier;
    frontier.emplace(0, api->getStart());

    // Dijkstra's algorithm is run.
    while(frontier.size() > 0) {
        Point frontPoint;
        uint frontCost;

        std::tie(frontCost, frontPoint) = frontier.top();
        frontier.pop();

        // A point might be in the frontier multiple times because the cost has
        // been updated. The up to date cost is necessarily in `pointInfo` so
        // it's safe to ignore anything that doesn't match the cost there.
        // This avoids the complexity of resorting or removing the redundant
        // points when the updated ones are inserted.
        while(frontCost != std::get<0>(pointInfo.at(frontPoint))) {
            std::tie(frontCost, frontPoint) = frontier.top();
            frontier.pop();
        }

        if(frontPoint == api->getFinish()) {
            break;
        }

        for(auto near : api->getNeighbors(frontPoint)) {
            Point nearPoint;
            Direction nearDir;

            std::tie(nearPoint, nearDir) = near;
            uint cost =
                std::get<0>(pointInfo.at(frontPoint)) + api->getMoveCost(frontPoint, nearDir);

            if(pointInfo.find(nearPoint) != pointInfo.end()) {
                if(cost < std::get<0>(pointInfo.at(nearPoint))) {
                    pointInfo[nearPoint] = infoTuple(cost, frontPoint, nearDir);
                    frontier.emplace(cost, nearPoint);
                }
            } else {
                pointInfo.emplace(nearPoint, infoTuple(cost, frontPoint, nearDir));
                frontier.emplace(cost, nearPoint);
            }
        }
    }

    // Reverse the path from the finish.
    std::vector<Direction> path;
    Point tracePoint = api->getFinish();
    Direction traceDir;

    while(tracePoint != api->getStart()) {
        std::tie(std::ignore, tracePoint, traceDir) = pointInfo.at(tracePoint);
        path.push_back(traceDir);
    }

    std::reverse(path.begin(), path.end());

    return path;
}
