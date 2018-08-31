#include <algorithm>
#include <cmath>
#include <queue>
#include <unordered_map>

#include "agent/agent-impl.h"

using Rally::MapInterface;
using Rally::Point;

namespace {

struct PointInfo {
  uint pathCost;
  uint estimate;
  Point parent;
  Direction::T parentDir;
  bool expanded;
};

inline uint hueristic(const Point& a, const Point& b) {
  return a.distanceTo(b) * 2;
}

}  // namespace

// This agent is a standard implementation of the A* algorithm.
REGISTER_AGENT(AStar)(MapInterface* const api) {
  const Point start = api->getStart();
  const Point finish = api->getFinish();
  std::unordered_map<Point, PointInfo> pointMap;
  pointMap.emplace(start,
                   PointInfo{
                       0,                         // pathCost
                       hueristic(start, finish),  // estimate
                       {-1, -1},                  // parent
                       Direction::T::eNone,       // parentDir
                       false                      // expanded
                   });

  std::priority_queue<std::pair<uint, Point>,
                      std::vector<std::pair<uint, Point>>,
                      std::greater<std::pair<uint, Point>>>
      frontier;
  frontier.emplace(pointMap.at(start).estimate, start);

  // A* algorithm is run.
  while(frontier.size() > 0) {
    // A point might be in the frontier multiple times because the cost has
    // been updated. The up to date cost is necessarily in `pointMap` so
    // it's safe to ignore anything that doesn't match the cost there.
    // This avoids the complexity of re-sorting or removing the redundant
    // points when the updated ones are inserted.
    uint frontWeight;
    Point frontPoint;
    PointInfo* frontInfo;
    do {
      frontWeight = frontier.top().first;
      frontPoint = frontier.top().second;
      frontier.pop();

      frontInfo = &pointMap.at(frontPoint);
    } while(frontWeight != frontInfo->pathCost + frontInfo->estimate);

    frontInfo->expanded = true;

    if(frontPoint == finish) {
      break;
    }

    for(const auto& near : api->getNeighbors(frontPoint)) {
      const Point nearPoint = near.first;
      const Direction::T nearDir = near.second;

      uint cost = frontInfo->pathCost + api->getMoveCost(frontPoint, nearDir);

      if(pointMap.find(nearPoint) != pointMap.end()) {
        PointInfo& nearInfo = pointMap.at(nearPoint);

        // If the point has been expanded the shortest distance is
        // already known.
        if(nearInfo.expanded) {
          continue;
        }

        if(cost < nearInfo.pathCost) {
          nearInfo = PointInfo{
              cost,               // pathCost
              nearInfo.estimate,  // estimate
              frontPoint,         // parent
              nearDir,            // parentDir
              false               // expanded
          };
          frontier.emplace(cost + nearInfo.estimate, nearPoint);
        }
      } else {
        const uint estimate = hueristic(nearPoint, finish);
        pointMap.emplace(nearPoint, PointInfo{
                                        cost,        // pathCost
                                        estimate,    // estimate
                                        frontPoint,  // parent
                                        nearDir,     // parentDir
                                        false        // expanded
                                    });
        frontier.emplace(cost + estimate, nearPoint);
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
