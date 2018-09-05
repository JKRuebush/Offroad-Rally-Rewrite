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
  uint pathEstimate;
  Point parent;
  Direction::T parentDir;
  bool expanded;
};

struct FrontierEntry {
  Point pos;
  uint shortestPathCost;
  uint pathEstimate;

  inline bool operator==(const FrontierEntry& rhs) {
    // At the moment there is no need to check `pathEstimate` as the position
    // being equal implies they are the same.
    return this->pos == rhs.pos &&
           this->shortestPathCost == rhs.shortestPathCost;
  }

  inline bool operator!=(const FrontierEntry& rhs) { return !operator==(rhs); }

  inline bool operator<(const FrontierEntry& rhs) const {
    if(this->shortestPathCost + this->pathEstimate ==
       rhs.shortestPathCost + rhs.pathEstimate) {
      // Note that this being greater than introduces bias.
      return this->shortestPathCost > rhs.shortestPathCost;
    }

    return this->shortestPathCost + this->pathEstimate <
           rhs.shortestPathCost + rhs.pathEstimate;
  }

  inline bool operator>(const FrontierEntry& rhs) const {
    return rhs.operator<(*this);
  }

  inline bool operator<=(const FrontierEntry& rhs) const {
    return !operator>(rhs);
  }

  inline bool operator>=(const FrontierEntry& rhs) const {
    return !operator<(rhs);
  }
};

inline uint hueristic(const Point& a, const uint& aRoughness, const Point& b) {
  return (a.distanceTo(b) - 1) * 2 + aRoughness + 1;
}

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
    const auto near = api->getDestination(pos, dir);

    if(near != pos) {
      neighbors.push_back({near, dir});
    }
  }

  return neighbors;
}

}  // namespace

// This agent is an implementation of the A* algorithm that takes more
// information about the specific problem being solved into account.
REGISTER_AGENT(AStarOpt)(MapInterface* const api) {
  const Point start = api->getStart();
  const Point finish = api->getFinish();

  // If the start is right next to the finish don't do extra work.
  for(const auto& hex : api->getNeighbors(start)) {
    if(hex.first == finish) {
      return std::vector<Direction::T>{hex.second};
    }
  }

  std::unordered_map<Point, PointInfo> pointMap;
  pointMap.insert({start,
                   PointInfo{
                       1,                            // roughness
                       0,                            // shortestPathCost
                       hueristic(start, 1, finish),  // pathEstimate
                       {-1, -1},                     // parent
                       Direction::T::eNone,          // parentDir
                       false                         // expanded
                   }});

  std::priority_queue<FrontierEntry, std::vector<FrontierEntry>,
                      std::greater<FrontierEntry>>
      frontier;
  frontier.push(FrontierEntry{start, 0, hueristic(start, 1, finish)});

  // A* algorithm is run.
  while(frontier.size() > 0) {
    // A point might be in the frontier multiple times because the cost has
    // been updated. The up to date cost is necessarily in `pointMap` so
    // it's safe to ignore anything that doesn't match the cost there.
    // This avoids the complexity of re-sorting or removing the redundant
    // points when the updated ones are inserted.
    uint frontCost;
    Point frontPoint;
    PointInfo* frontInfo;
    do {
      frontPoint = frontier.top().pos;
      frontInfo = &pointMap.at(frontPoint);
      frontCost = frontier.top().shortestPathCost;

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
          nearInfo.shortestPathCost = shortestPathCost;
          nearInfo.parent = frontPoint;
          nearInfo.parentDir = nearDir;

          frontier.push(FrontierEntry{nearPoint, shortestPathCost,
                                      nearInfo.pathEstimate});
        }
      } else {
        const uint moveCost = api->getMoveCost(frontPoint, nearDir);
        const uint nearRoughness = moveCost - frontInfo->roughness;
        const uint pathEstimate = hueristic(nearPoint, nearRoughness, finish);
        const uint shortestPathCost = moveCost + frontInfo->shortestPathCost;

        pointMap.insert(
            {nearPoint, PointInfo{nearRoughness, shortestPathCost, pathEstimate,
                                  frontPoint, nearDir, false}});
        frontier.push(FrontierEntry{nearPoint, shortestPathCost, pathEstimate});
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
