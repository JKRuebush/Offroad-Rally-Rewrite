#include <algorithm>
#include <cmath>
#include <map>
#include <queue>

#include "rallyAgent.h"

namespace {
struct PointInfo {
    uint pathCost;
    Point parent;
    Direction parentDir;
    bool expanded;
};
}  // namespace

REGISTER_AGENT(Dijkstra)(MapInterface* const api) {
    Point start = api->getStart();
    Point finish = api->getFinish();
    std::map<Point, PointInfo> pointMap;
    pointMap.emplace(start,
                     PointInfo{
                         0,                // pathCost
                         Point(-1, -1),    // parent
                         Direction::None,  // parentDir
                         false             // expanded
                     });

    std::priority_queue<std::pair<uint, Point>, std::vector<std::pair<uint, Point>>,
                        std::greater<std::pair<uint, Point>>>
        frontier;
    frontier.emplace(0, start);

    // Dijkstra's algorithm is run.
    while(frontier.size() > 0) {
        uint frontCost = frontier.top().first;
        Point frontPoint = frontier.top().second;
        frontier.pop();

        // A point might be in the frontier multiple times because the cost has
        // been updated. The up to date cost is necessarily in `pointMap` so
        // it's safe to ignore anything that doesn't match the cost there.
        // This avoids the complexity of resorting or removing the redundant
        // points when the updated ones are inserted.
        PointInfo frontInfo = pointMap.at(frontPoint);
        while(frontCost != frontInfo.pathCost) {
            frontCost = frontier.top().first;
            frontPoint = frontier.top().second;
            frontier.pop();
            frontInfo = pointMap.at(frontPoint);
        }

        frontInfo.expanded = true;

        if(frontPoint == finish) {
            break;
        }

        for(auto near : api->getNeighbors(frontPoint)) {
            Point nearPoint = near.first;
            Direction nearDir = near.second;

            uint cost = frontInfo.pathCost + api->getMoveCost(frontPoint, nearDir);

            if(pointMap.find(nearPoint) != pointMap.end()) {
                PointInfo& nearInfo = pointMap.at(nearPoint);

                // If the point has been expanded the shortest distance is
                // already known.
                if(nearInfo.expanded) {
                    continue;
                }

                if(cost < nearInfo.pathCost) {
                    nearInfo = PointInfo{
                        cost,        // pathCost
                        frontPoint,  // parent
                        nearDir,     // parentDir
                        false        // expanded
                    };
                    frontier.emplace(cost, nearPoint);
                }
            } else {
                pointMap.emplace(nearPoint, PointInfo{
                                                cost,        // pathCost
                                                frontPoint,  // parent
                                                nearDir,     // parentDir
                                                false        // expanded
                                            });
                frontier.emplace(cost, nearPoint);
            }
        }
    }

    // Reverse the path from the finish.
    std::vector<Direction> path;
    Point tracePoint = finish;
    Direction traceDir;

    while(tracePoint != start) {
        PointInfo info = pointMap.at(tracePoint);
        tracePoint = info.parent;
        traceDir = info.parentDir;
        path.push_back(traceDir);
    }

    std::reverse(path.begin(), path.end());

    return path;
}
