#include <algorithm>
#include <cmath>
#include <queue>
#include <unordered_map>

#include "agent/agent-impl.h"

using Rally::MapInterface;
using Rally::Point;

namespace {

struct PointInfo {
  uint roughness;
  uint shortestPathCost;
  Point parent;
  Direction::T parentDir;
  bool expanded;
};

// All points other than the start have another point before them in the path.
// There is never a lower cost for backtracking, so it's wasted effort to check
// points that the previous point already checked. This cuts out those extra
// map looks.
std::vector<std::pair<Point, Direction::T>> getRelevantNeighbors(
    Point pos,
    Direction::T parentDir,
    MapInterface* const api) {
  std::vector<Direction::T> directions;

  switch(parentDir) {
    case Direction::T::eNorth:
      directions = {Direction::T::eNorthWest, Direction::T::eNorth,
                    Direction::T::eNorthEast};
      break;
    case Direction::T::eNorthEast:
      directions = {Direction::T::eNorth, Direction::T::eNorthEast,
                    Direction::T::eSouthEast};
      break;
    case Direction::T::eSouthEast:
      directions = {Direction::T::eNorthEast, Direction::T::eSouthEast,
                    Direction::T::eSouth};
      break;
    case Direction::T::eSouth:
      directions = {Direction::T::eSouthEast, Direction::T::eSouth,
                    Direction::T::eSouthWest};
      break;
    case Direction::T::eSouthWest:
      directions = {Direction::T::eSouth, Direction::T::eSouthWest,
                    Direction::T::eNorthWest};
      break;
    case Direction::T::eNorthWest:
      directions = {Direction::T::eSouthWest, Direction::T::eNorthWest,
                    Direction::T::eNorth};
      break;
    case Direction::T::eNone:
      directions.assign(Direction::kAllMoveDirections.begin(),
                        Direction::kAllMoveDirections.end());
  }

  std::vector<std::pair<Point, Direction::T>> neighbors;
  neighbors.reserve(3);
  for(const auto& dir : directions) {
    auto near = api->getDestination(pos, dir);

    if(near != pos) {
      neighbors.push_back({near, dir});
    }
  }

  return neighbors;
}

}  // namespace

// This agent is an implementation of Dijkstra's algorithm that takes more
// information about the specific problem being solved into account.
REGISTER_AGENT(DijkstraOpt)(MapInterface* const api) {
  const Point start = api->getStart();
  const Point finish = api->getFinish();
  std::unordered_map<Point, PointInfo> pointMap;
  pointMap.insert({start,
                   PointInfo{
                       1,                    // roughness
                       0,                    // shortestPathCost
                       {-1, -1},             // parent
                       Direction::T::eNone,  // parentDir
                       false                 // expanded
                   }});

  std::priority_queue<std::pair<uint, Point>,
                      std::vector<std::pair<uint, Point>>,
                      std::greater<std::pair<uint, Point>>>
      frontier;
  frontier.push({0, start});

  // Dijkstra's algorithm is run.
  while(frontier.size() > 0) {
    // A point might be in the frontier multiple times because the cost has
    // been updated. The up to date cost is necessarily in `pointMap` so
    // it's safe to ignore anything that doesn't match the cost there.
    // This avoids the complexity of resorting or removing the redundant
    // points when the updated ones are inserted.
    uint frontCost;
    Point frontPoint;
    PointInfo* frontInfo;
    do {
      frontPoint = frontier.top().second;
      frontInfo = &pointMap.at(frontPoint);
      frontCost = frontier.top().first;

      frontier.pop();

    } while(!frontInfo->expanded && frontCost != frontInfo->shortestPathCost);

    frontInfo->expanded = true;

    if(frontPoint == finish) {
      break;
    }

    for(const auto& near :
        getRelevantNeighbors(frontPoint, frontInfo->parentDir, api)) {
      const Point nearPoint = near.first;
      const Direction::T nearDir = near.second;

      if(pointMap.find(nearPoint) != pointMap.end()) {
        PointInfo& nearInfo = pointMap.at(nearPoint);

        // If the point has been expanded the shortest distance is
        // already known.
        if(nearInfo.expanded) {
          continue;
        }

        uint shortestPathCost = frontInfo->shortestPathCost +
                                frontInfo->roughness + nearInfo.roughness;

        if(shortestPathCost < nearInfo.shortestPathCost) {
          nearInfo = PointInfo{nearInfo.roughness, shortestPathCost, frontPoint,
                               nearDir, false};
          frontier.push({shortestPathCost, nearPoint});
        }
      } else {
        uint moveCost = api->getMoveCost(frontPoint, nearDir);
        uint nearRoughness = moveCost - frontInfo->roughness;
        uint shortestPathCost = moveCost + frontInfo->shortestPathCost;

        pointMap.insert({nearPoint, PointInfo{nearRoughness, shortestPathCost,
                                              frontPoint, nearDir, false}});
        frontier.push({shortestPathCost, nearPoint});
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
