#include <algorithm>
#include <cmath>
#include <queue>
#include <set>
#include <unordered_map>

#include "agent/agent-impl.h"

using Rally::MapInterface;
using Rally::Point;

namespace {

typedef std::priority_queue<std::pair<uint, Point>,
                            std::vector<std::pair<uint, Point>>,
                            std::greater<std::pair<uint, Point>>>
    FrontierQueue;

struct PointInfo {
  uint shortestPathCost;
  uint pathEstimate;
  Point parent;
  Direction::T parentDir;
};

inline uint hueristic(const Point& a, const Point& b) {
  return a.distanceTo(b) * 2;
}

// A point might be in the frontier multiple times because the cost has been
// updated, or it might have been moved into the closed set. The pointMap holds
// the up to date information, so it's simple to clear out the invalid date.
// Simply clearing off the top of the frontier is preferable to sorting and
// validating the frontier as points are added.
void clearFrontierTop(FrontierQueue& frontier,
                      const std::unordered_map<Point, PointInfo>& pointMap,
                      const std::set<Point>& closed) {
  if(frontier.size() == 0) {
    return;
  }

  Point frontPoint = frontier.top().second;
  const PointInfo* frontInfo = &pointMap.at(frontPoint);
  uint frontCost = frontier.top().first - frontInfo->pathEstimate;

  while(frontier.size() > 0 && (closed.find(frontPoint) != closed.end() ||
                                frontCost != frontInfo->shortestPathCost)) {
    frontier.pop();

    frontPoint = frontier.top().second;
    frontInfo = &pointMap.at(frontPoint);
    frontCost = frontier.top().first - frontInfo->pathEstimate;
  };
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
  clearFrontierTop(frontier, pointMapA, closed);

  if(frontier.size() == 0) {
    return;
  }

  const Point frontPoint = frontier.top().second;
  const PointInfo& frontInfo = pointMapA.at(frontPoint);

  closed.insert(frontPoint);

  // A point is considered only if the pathEstimated cost to reach the end is
  // less than the known shortest path to reach the end.
  if(frontInfo.shortestPathCost + frontInfo.pathEstimate < shortestFullPath &&
     frontInfo.shortestPathCost + shortestPathB -
             hueristic(frontPoint, source) <
         shortestFullPath) {
    for(const auto& near : api->getNeighbors(frontPoint)) {
      Point nearPoint = near.first;
      Direction::T nearDir = near.second;

      uint pathCost =
          frontInfo.shortestPathCost + api->getMoveCost(frontPoint, nearDir);

      if(pointMapA.find(nearPoint) != pointMapA.end()) {
        PointInfo& nearInfo = pointMapA.at(nearPoint);

        if(pathCost < nearInfo.shortestPathCost) {
          nearInfo.shortestPathCost = pathCost;
          nearInfo.parent = frontPoint;
          nearInfo.parentDir = nearDir;
          frontier.push({pathCost + nearInfo.pathEstimate, nearPoint});

          // Check if the frontiers are touching. If so update the shortest path
          // as needed.
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
        const uint pathEstimate = hueristic(nearPoint, target);

        pointMapA.insert({nearPoint, PointInfo{pathCost, pathEstimate,
                                               frontPoint, nearDir}});
        frontier.push({pathCost + pathEstimate, nearPoint});

        if(pointMapB.find(nearPoint) != pointMapB.end()) {
          uint combinedCost =
              pathCost + pointMapB.at(nearPoint).shortestPathCost;

          if(combinedCost < shortestFullPath) {
            shortestFullPath = combinedCost;
            touchPoint = nearPoint;
          }
        }
      }
    }
  }

  // This clear is technically unneeded, but results in less map looks.
  clearFrontierTop(frontier, pointMapA, closed);

  if(frontier.size() > 0) {
    shortestPathA = frontier.top().first;
  }
}
}  // namespace

/*
The algorithm implemented here is the New Bidirectional A* algorithm (NBA*).
This algorithm is presented in "Yet another bidirectional algorithm for shortest
paths" by Wim Pijls and Henk Post.
*/
REGISTER_AGENT(NBAStar)(MapInterface* const api) {
  const Point start = api->getStart();
  const Point finish = api->getFinish();

  std::unordered_map<Point, PointInfo> pointMapForwards;
  pointMapForwards.insert({start, PointInfo{
                                      0,  // shortestPathCost
                                      hueristic(start, finish),  // pathEstimate
                                      {-1, -1},                  // parent
                                      Direction::T::eNone        // parentDir
                                  }});

  std::unordered_map<Point, PointInfo> pointMapBackwards;
  pointMapBackwards.insert(
      {finish, PointInfo{
                   0,                         // shortestPathCost
                   hueristic(finish, start),  // pathEstimate
                   {-1, -1},                  // parent
                   Direction::T::eNone        // parentDir
               }});

  std::set<Point> closed;

  FrontierQueue frontierForwards;
  frontierForwards.push({hueristic(finish, start), start});

  FrontierQueue frontierBackwards;
  frontierBackwards.push({hueristic(start, finish), finish});

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
