#include <algorithm>
#include <cmath>
#include <map>
#include <queue>

#include "rallyAgent.h"

namespace {
struct PointInfo {
    uint roughness;
    uint pathCost;
    uint estimate;
    Point parent;
    Direction parentDir;
    bool expanded;
};

std::vector<std::pair<Point, Direction>> getRelevantNeighbors(Point pos,
                                                              Direction parentDir,
                                                              MapInterface* const api) {
    std::vector<Direction> directions;

    switch(parentDir) {
        case Direction::North:
            directions = {Direction::NorthWest, Direction::North, Direction::NorthEast};
            break;
        case Direction::NorthEast:
            directions = {Direction::North, Direction::NorthEast, Direction::SouthEast};
            break;
        case Direction::SouthEast:
            directions = {Direction::NorthEast, Direction::SouthEast, Direction::South};
            break;
        case Direction::South:
            directions = {Direction::SouthEast, Direction::South, Direction::SouthWest};
            break;
        case Direction::SouthWest:
            directions = {Direction::South, Direction::SouthWest, Direction::NorthWest};
            break;
        case Direction::NorthWest:
            directions = {Direction::SouthWest, Direction::NorthWest, Direction::North};
            break;
        case Direction::None:
            directions.assign(DIRECTIONS, DIRECTIONS + sizeof(DIRECTIONS) / sizeof(Direction));
    }

    std::vector<std::pair<Point, Direction>> neighbors;
    for(auto dir : directions) {
        auto near = api->getDestination(pos, dir);

        if(near != pos) {
            neighbors.emplace_back(near, dir);
        }
    }

    return neighbors;
}
}  // namespace

REGISTER_AGENT(AStarOpt)(MapInterface* const api) {
    Point start = api->getStart();
    Point finish = api->getFinish();
    std::map<Point, PointInfo> pointMap;
    pointMap.emplace(start,
                     PointInfo{
                         1,                             // roughness
                         0,                             // pathCost
                         start.distanceTo(finish) * 2,  // estimate
                         Point(-1, -1),                 // parent
                         Direction::None,               // parentDir
                         false                          // expanded
                     });

    std::priority_queue<std::pair<uint, Point>, std::vector<std::pair<uint, Point>>,
                        std::greater<std::pair<uint, Point>>>
        frontier;
    frontier.emplace(pointMap.at(start).estimate, start);

    // A* algorithm is run.
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
        while(frontCost != frontInfo.pathCost + frontInfo.estimate) {
            frontCost = frontier.top().first;
            frontPoint = frontier.top().second;
            frontier.pop();
            frontInfo = pointMap.at(frontPoint);
        }

        frontInfo.expanded = true;

        if(frontPoint == finish) {
            break;
        }

        for(auto near : getRelevantNeighbors(frontPoint, frontInfo.parentDir, api)) {
            Point nearPoint = near.first;
            Direction nearDir = near.second;

            if(pointMap.find(nearPoint) != pointMap.end()) {
                PointInfo& nearInfo = pointMap.at(nearPoint);

                // If the point has been expanded the shortest distance is
                // already known.
                if(nearInfo.expanded) {
                    continue;
                }

                uint pathCost = frontInfo.pathCost + frontInfo.roughness + nearInfo.roughness;

                if(pathCost < nearInfo.pathCost) {
                    nearInfo = PointInfo{nearInfo.roughness, pathCost, nearInfo.estimate,
                                         frontPoint,         nearDir,  false};
                    frontier.emplace(pathCost + nearInfo.estimate, nearPoint);
                }
            } else {
                uint estimate = nearPoint.distanceTo(finish) * 2;
                uint moveCost = api->getMoveCost(frontPoint, nearDir);
                uint nearRoughness = moveCost - frontInfo.roughness;
                uint pathCost = moveCost + frontInfo.pathCost;

                pointMap.emplace(nearPoint, PointInfo{nearRoughness, pathCost, estimate, frontPoint,
                                                      nearDir, false});
                frontier.emplace(pathCost + estimate, nearPoint);
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
