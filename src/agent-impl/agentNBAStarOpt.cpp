#include <algorithm>
#include <cmath>
#include <queue>
#include <set>
#include <unordered_map>

#include "agent/agent-impl.h"

using Rally::MapInterface;
using Rally::Point;

namespace {

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
      return this->shortestPathCost < rhs.shortestPathCost;
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

typedef std::priority_queue<FrontierEntry,
                            std::vector<FrontierEntry>,
                            std::greater<FrontierEntry>>
    FrontierQueue;

struct PointInfo {
  uint roughness;
  uint shortestPathCost;
  uint pathEstimate;
  Point parent;
  Direction::T parentDir;
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

void expandFrontier(MapInterface* const api,
                    std::unordered_map<Point, PointInfo>& pointMapA,
                    const std::unordered_map<Point, PointInfo>& pointMapB,
                    std::set<Point>& closed,
                    FrontierQueue& frontier,
                    const Point& source,
                    const Point& target,
                    Point& touchPoint,
                    uint& shortestFullPath,
                    uint& shortestPathA,
                    uint shortestPathB) {
  // A point might be in the frontier multiple times because the cost has
  // been updated. The up to date cost is necessarily in `pointMapA` so
  // it's safe to ignore anything that doesn't match the cost there.
  // This avoids the complexity of resorting or removing the redundant
  // points when the updated ones are inserted.
  uint frontCost;
  Point frontPoint;
  PointInfo* frontInfo;
  do {
    frontPoint = frontier.top().pos;
    frontInfo = &pointMapA.at(frontPoint);
    frontCost = frontier.top().shortestPathCost;

    if(frontier.size() == 0) {
      return;
    }

    frontier.pop();

  } while(closed.find(frontPoint) != closed.end() &&
          frontCost != frontInfo->shortestPathCost);

  closed.insert(frontPoint);

  // A point is considered only if the pathEstimated cost to reach the end is
  // less than the known shortest path to reach the end.
  if(frontInfo->shortestPathCost + frontInfo->pathEstimate < shortestFullPath &&
     frontInfo->shortestPathCost + shortestPathB -
             hueristic(frontPoint, frontInfo->roughness, source) <
         shortestFullPath) {
    for(const auto& near :
        getRelevantNeighbors(frontPoint, frontInfo->parentDir, api)) {
      Point nearPoint = near.first;
      Direction::T nearDir = near.second;

      if(pointMapA.find(nearPoint) != pointMapA.end()) {
        PointInfo& nearInfo = pointMapA.at(nearPoint);

        uint cost = frontInfo->shortestPathCost + frontInfo->roughness +
                    nearInfo.roughness;

        if(cost < nearInfo.shortestPathCost) {
          nearInfo.shortestPathCost = cost;
          nearInfo.parent = frontPoint;
          nearInfo.parentDir = nearDir;
          frontier.push({nearPoint, cost, nearInfo.pathEstimate});

          // Check if the frontiers are touching. If so update the shortest
          // path as needed.
          if(pointMapB.find(nearPoint) != pointMapB.end()) {
            uint combinedCost = nearInfo.shortestPathCost +
                                pointMapB.at(nearPoint).shortestPathCost;

            if(combinedCost < shortestFullPath) {
              shortestFullPath = combinedCost;
              touchPoint = nearPoint;
            }
          }
        }
      } else {
        const uint moveCost = api->getMoveCost(frontPoint, nearDir);
        const uint nearRoughness = moveCost - frontInfo->roughness;
        const uint pathEstimate = hueristic(nearPoint, nearRoughness, target);
        const uint shortestPathCost = moveCost + frontInfo->shortestPathCost;

        pointMapA.insert(
            {nearPoint, PointInfo{nearRoughness, shortestPathCost, pathEstimate,
                                  frontPoint, nearDir}});
        frontier.push(FrontierEntry{nearPoint, shortestPathCost, pathEstimate});

        if(pointMapB.find(nearPoint) != pointMapB.end()) {
          uint combinedCost =
              shortestPathCost + pointMapB.at(nearPoint).shortestPathCost;

          if(combinedCost < shortestFullPath) {
            shortestFullPath = combinedCost;
            touchPoint = nearPoint;
          }
        }
      }
    }
  }

  shortestPathA = frontier.top().shortestPathCost + frontier.top().pathEstimate;
}
}  // namespace

REGISTER_AGENT(NBAStarOpt)(MapInterface* const api) {
  const Point start = api->getStart();
  const Point finish = api->getFinish();

  // If the start is right next to the finish don't do extra work.
  for(const auto& hex : api->getNeighbors(start)) {
    if(hex.first == finish) {
      return std::vector<Direction::T>{hex.second};
    }
  }

  std::unordered_map<Point, PointInfo> pointMapForwards;
  pointMapForwards.insert(
      {start, PointInfo{
                  1,                            // roughness
                  0,                            // shortestPathCost
                  hueristic(start, 1, finish),  // pathEstimate
                  {-1, -1},                     // parent
                  Direction::T::eNone           // parentDir
              }});

  std::unordered_map<Point, PointInfo> pointMapBackwards;
  pointMapBackwards.insert(
      {finish, PointInfo{
                   1,                            // roughness
                   0,                            // shortestPathCost
                   hueristic(finish, 1, start),  // pathEstimate
                   {-1, -1},                     // parent
                   Direction::T::eNone           // parentDir
               }});

  std::set<Point> closed;

  FrontierQueue frontierForwards;
  frontierForwards.push(FrontierEntry{start, 0, hueristic(finish, 1, start)});

  FrontierQueue frontierBackwards;
  frontierBackwards.push(FrontierEntry{finish, 0, hueristic(start, 1, finish)});

  Point touchPoint = {-1, -1};
  uint shortestFullPath = ~0;
  uint shortestPathForwards = pointMapForwards.at(start).pathEstimate;
  uint shortestPathBackwards = pointMapBackwards.at(finish).pathEstimate;

  while(frontierForwards.size() > 0 && frontierBackwards.size() > 0) {
    if(frontierForwards.size() <= frontierBackwards.size()) {
      expandFrontier(api, pointMapForwards, pointMapBackwards, closed,
                     frontierForwards, start, finish, touchPoint,
                     shortestFullPath, shortestPathForwards,
                     shortestPathBackwards);
    } else {
      expandFrontier(api, pointMapBackwards, pointMapForwards, closed,
                     frontierBackwards, finish, start, touchPoint,
                     shortestFullPath, shortestPathBackwards,
                     shortestPathForwards);
    }
  }

  // Put together forwards half of the path.
  std::vector<Direction::T> path;
  Point tracePoint = touchPoint;
  Direction::T traceDir;

  while(tracePoint != start) {
    PointInfo info = pointMapForwards.at(tracePoint);
    tracePoint = info.parent;
    traceDir = info.parentDir;
    path.push_back(traceDir);
  }

  std::reverse(path.begin(), path.end());

  // Put together backwards half of the path.
  tracePoint = touchPoint;
  while(tracePoint != finish) {
    PointInfo info = pointMapBackwards.at(tracePoint);
    tracePoint = info.parent;
    traceDir = info.parentDir;
    path.push_back(Direction::reverse(traceDir));
  }

  return path;
}
