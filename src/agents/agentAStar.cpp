#include <algorithm>
#include <cmath>
#include <map>
#include <queue>
#include <tuple>

#include "rallyAgent.h"

namespace {
// In order this tuple holds: the length of the shortest path found to the
// point, a cost estimate for how long it left to the end, the previous point
// in the path, and the direction the previous point moved in to get to the
// current point.
typedef std::tuple<uint, uint, Point, Direction> infoTuple;
}  // namespace

REGISTER_AGENT(AStar)(MapInterface* const api) {
    Point start = api->getStart();
    Point finish = api->getFinish();
    std::map<Point, infoTuple> pointInfo;
    pointInfo.emplace(start,
                      infoTuple(0, start.distanceTo(finish) * 2, Point(-1, -1), Direction::North));

    std::priority_queue<std::pair<uint, Point>, std::vector<std::pair<uint, Point>>,
                        std::greater<std::pair<uint, Point>>>
        frontier;
    frontier.emplace(std::get<1>(pointInfo.at(start)), start);

    // A* algorithm is run.
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
        auto frontInfo = pointInfo.at(frontPoint);
        while(frontCost != std::get<0>(frontInfo) + std::get<1>(frontInfo)) {
            std::tie(frontCost, frontPoint) = frontier.top();
            frontier.pop();
            frontInfo = pointInfo.at(frontPoint);
        }

        if(frontPoint == finish) {
            break;
        }

        for(auto near : api->getNeighbors(frontPoint)) {
            Point nearPoint;
            Direction nearDir;

            std::tie(nearPoint, nearDir) = near;
            uint cost =
                std::get<0>(pointInfo.at(frontPoint)) + api->getMoveCost(frontPoint, nearDir);

            if(pointInfo.find(nearPoint) != pointInfo.end()) {
                auto nearInfo = pointInfo.at(nearPoint);
                if(cost < std::get<0>(nearInfo)) {
                    pointInfo[nearPoint] =
                        infoTuple(cost, std::get<1>(nearInfo), frontPoint, nearDir);
                    frontier.emplace(cost + std::get<1>(nearInfo), nearPoint);
                }
            } else {
                uint estimate = nearPoint.distanceTo(finish) * 2;
                pointInfo.emplace(nearPoint, infoTuple(cost, estimate, frontPoint, nearDir));
                frontier.emplace(cost + estimate, nearPoint);
            }
        }
    }

    // Reverse the path from the finish.
    std::vector<Direction> path;
    Point tracePoint = api->getFinish();
    Direction traceDir;

    while(tracePoint != start) {
        std::tie(std::ignore, std::ignore, tracePoint, traceDir) = pointInfo.at(tracePoint);
        path.push_back(traceDir);
    }

    std::reverse(path.begin(), path.end());

    return path;
}
