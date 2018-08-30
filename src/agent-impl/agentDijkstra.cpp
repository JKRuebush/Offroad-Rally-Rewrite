#include <algorithm>
#include <cmath>
#include <map>
#include <queue>

#include "agent/agent-impl.h"

using Rally::MapInterface;
using Rally::Point;

namespace {
struct PointInfo {
  uint pathCost;
  Point parent;
  Direction::T parentDir;
  bool expanded;
};
}  // namespace

// This is a standard implementation of Dijkstra's algorithm.
REGISTER_AGENT(Dijkstra)(MapInterface* const api) {
  const Point start = api->getStart();
  const Point finish = api->getFinish();
  std::map<Point, PointInfo> pointMap;
  pointMap.emplace(start,
                   PointInfo{
                       0,                    // pathCost
                       {-1, -1},             // parent
                       Direction::T::eNone,  // parentDir
                       false                 // expanded
                   });

  std::priority_queue<std::pair<uint, Point>,
                      std::vector<std::pair<uint, Point>>,
                      std::greater<std::pair<uint, Point>>>
      frontier;
  frontier.emplace(0, start);

  // Dijkstra's algorithm is run.
  while(frontier.size() > 0) {
    // A point might be in the frontier multiple times because the cost has
    // been updated. The up to date cost is necessarily in `pointMap` so
    // it's safe to ignore anything that doesn't match the cost there.
    // This avoids the complexity of resorting or removing the redundant
    // points when the updated ones are inserted.
    uint frontWeight;
    Point frontPoint;
    PointInfo* frontInfo;
    do {
      frontWeight = frontier.top().first;
      frontPoint = frontier.top().second;
      frontier.pop();

      frontInfo = &pointMap.at(frontPoint);
    } while(frontWeight != frontInfo->pathCost);

    frontInfo->expanded = true;

    if(frontPoint == finish) {
      break;
    }

    for(const auto& near : api->getNeighbors(frontPoint)) {
      const Point nearPoint = near.first;
      const Direction::T nearDir = near.second;

      const uint cost =
          frontInfo->pathCost + api->getMoveCost(frontPoint, nearDir);

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
  std::vector<Direction::T> path;
  Point tracePoint = finish;
  Direction::T traceDir;

  while(tracePoint != start) {
    PointInfo info = pointMap.at(tracePoint);
    tracePoint = info.parent;
    traceDir = info.parentDir;
    path.push_back(traceDir);
  }

  std::reverse(path.begin(), path.end());

  return path;
}
