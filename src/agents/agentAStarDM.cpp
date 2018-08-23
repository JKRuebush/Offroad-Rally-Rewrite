#include <algorithm>
#include <cmath>
#include <map>
#include <queue>
#include <tuple>

#include "rallyAgent.h"

namespace {
// In order this tuple holds: the roughness of the point, the length of the
// shortest path found to the point, a cost estimate for how long it left to the
// end, the previous point in the path, and the direction the previous point
// moved in to get to the current point.
typedef std::tuple<uint, uint, uint, Point, Direction> infoTuple;
}  // namespace

// StarDM is both directionally aware, and tracks roughness values. This does
// not result in less map looks, but overall takes less time to run.
REGISTER_AGENT(AStarDM)(MapInterface* const api) {
    Point start = api->getStart();
    Point finish = api->getFinish();
    std::map<Point, infoTuple> pointInfo;
    pointInfo.emplace(
        start, infoTuple(1, 0, start.distanceTo(finish) * 2, Point(-1, -1), Direction::None));

    std::priority_queue<std::pair<uint, Point>, std::vector<std::pair<uint, Point>>,
                        std::greater<std::pair<uint, Point>>>
        frontier;
    frontier.emplace(std::get<2>(pointInfo.at(start)), start);

    // A* algorithm is run.
    while(frontier.size() > 0) {
        Point frontPoint;
        uint frontWeighted;

        std::tie(frontWeighted, frontPoint) = frontier.top();
        frontier.pop();

        // A point might be in the frontier multiple times because the cost has
        // been updated. The up to date cost is necessarily in `pointInfo` so
        // it's safe to ignore anything that doesn't match the cost there.
        // This avoids the complexity of resorting or removing the redundant
        // points when the updated ones are inserted.
        auto frontInfo = pointInfo.at(frontPoint);
        while(frontWeighted != std::get<1>(frontInfo) + std::get<2>(frontInfo)) {
            std::tie(frontWeighted, frontPoint) = frontier.top();
            frontier.pop();
            frontInfo = pointInfo.at(frontPoint);
        }

        if(frontPoint == finish) {
            break;
        }

        // These directions can all be ignored. The previous point in the path
        // already looked at the points in these directions. It will never be
        // Shorter to backtrack.
        Direction parentDir = reverseDirection(std::get<4>(pointInfo.at(frontPoint)));
        Direction parentDirLeft = rotateDirectionLeft(parentDir);
        Direction parentDirRight = rotateDirectionRight(parentDir);

        for(auto near : api->getNeighbors(frontPoint)) {
            Point nearPoint;
            Direction nearDir;

            std::tie(nearPoint, nearDir) = near;

            if(nearDir == parentDir || nearDir == parentDirLeft || nearDir == parentDirRight) {
                continue;
            }

            uint frontRough = std::get<0>(pointInfo.at(frontPoint));
            uint frontCost = std::get<1>(pointInfo.at(frontPoint));

            if(pointInfo.find(nearPoint) != pointInfo.end()) {
                uint storedRough;
                uint storedCost;
                uint storedEstimate;

                std::tie(storedRough, storedCost, storedEstimate, std::ignore, std::ignore) =
                    pointInfo.at(nearPoint);

                uint pathCost = storedRough + frontRough + frontCost;

                if(pathCost < storedCost) {
                    pointInfo[nearPoint] =
                        infoTuple(storedRough, pathCost, storedEstimate, frontPoint, nearDir);
                    frontier.emplace(pathCost + storedEstimate, nearPoint);
                }
            } else {
                uint estimate = nearPoint.distanceTo(finish) * 2;

                uint moveCost = api->getMoveCost(frontPoint, nearDir);
                uint roughness = moveCost - frontRough;
                uint pathCost = moveCost + frontCost;

                pointInfo.emplace(nearPoint,
                                  infoTuple(roughness, pathCost, estimate, frontPoint, nearDir));
                frontier.emplace(pathCost + estimate, nearPoint);
            }
        }
    }

    // Reverse the path from the finish.
    std::vector<Direction> path;
    Point tracePoint = api->getFinish();
    Direction traceDir;

    while(tracePoint != start) {
        std::tie(std::ignore, std::ignore, std::ignore, tracePoint, traceDir) =
            pointInfo.at(tracePoint);
        path.push_back(traceDir);
    }

    std::reverse(path.begin(), path.end());

    return path;
}
