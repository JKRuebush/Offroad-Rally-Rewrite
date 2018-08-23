#include <algorithm>
#include <cmath>
#include <map>
#include <queue>
#include <tuple>

#include "rallyAgent.h"

namespace {
// In order this tuple holds: the roughness at that point, the length of the
// shortest path found to the point, the previous point in the path, and the
// direction the previous point moved in to get to the current point.
typedef std::tuple<uint, uint, Point, Direction> infoTuple;
}  // namespace

// DijkstraDM is both directionally aware, and tracks roughness values. This
// does not result in less map looks, but overall takes less time to run.
REGISTER_AGENT(DijkstraDM)(MapInterface* const api) {
    std::map<Point, infoTuple> pointInfo;
    pointInfo.emplace(api->getStart(), infoTuple(1, 0, Point(-1, -1), Direction::None));

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
        while(frontCost != std::get<1>(pointInfo.at(frontPoint))) {
            std::tie(frontCost, frontPoint) = frontier.top();
            frontier.pop();
        }

        if(frontPoint == api->getFinish()) {
            break;
        }

        // These directions can all be ignored. The previous point in the path
        // already looked at the points in these directions. It will never be
        // Shorter to backtrack.
        Direction parentDir = reverseDirection(std::get<3>(pointInfo.at(frontPoint)));
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

            if(pointInfo.find(nearPoint) != pointInfo.end()) {
                uint storedCost;
                uint storedRough;

                std::tie(storedRough, storedCost, std::ignore, std::ignore) =
                    pointInfo.at(nearPoint);

                uint pathCost = storedRough + frontRough + frontCost;

                if(pathCost < storedCost) {
                    pointInfo[nearPoint] = infoTuple(storedRough, pathCost, frontPoint, nearDir);
                    frontier.emplace(pathCost, nearPoint);
                }
            } else {
                uint moveCost = api->getMoveCost(frontPoint, nearDir);
                uint roughness = moveCost - frontRough;
                uint pathCost = moveCost + frontCost;

                pointInfo.emplace(nearPoint, infoTuple(roughness, pathCost, frontPoint, nearDir));
                frontier.emplace(pathCost, nearPoint);
            }
        }
    }

    // Reverse the path from the finish.
    std::vector<Direction> path;
    Point tracePoint = api->getFinish();
    Direction traceDir;

    while(tracePoint != api->getStart()) {
        std::tie(std::ignore, std::ignore, tracePoint, traceDir) = pointInfo.at(tracePoint);
        path.push_back(traceDir);
    }

    std::reverse(path.begin(), path.end());

    return path;
}
