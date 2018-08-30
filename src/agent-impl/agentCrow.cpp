#include <cmath>

#include "agent/agent-impl.h"

using Rally::MapInterface;
using Rally::Point;

namespace {

Point hexRound(double x, double y) {
  double roundedX = round(x);
  double roundedY = round(y);
  double roundedZ = round(-x - y);

  double deltaX = std::fabs(roundedX - x);
  double deltaY = std::fabs(roundedY - y);
  double deltaZ = std::fabs(roundedZ + x + y);

  // Floating point error shouldn't be an issue here as a large amount of
  // precision isn't needed.
  if(deltaX > deltaY && deltaX > deltaZ) {
    roundedX = -roundedY - roundedZ;
  } else if(deltaY > deltaZ) {
    roundedY = -roundedX - roundedZ;
  }

  return {static_cast<int>(roundedX), static_cast<int>(roundedY)};
}

inline double lerp(int a, int b, double t) {
  return static_cast<double>(a) + static_cast<double>(b - a) * t;
}

inline Point hexLerp(Point a, Point b, double t) {
  return hexRound(lerp(a.x, b.x, t), lerp(a.y, b.y, t));
}

}  // namespace

// This agent takes the straight path towards the finish line.
REGISTER_AGENT(Crow)(MapInterface* const api) {
  const Point start = api->getStart();
  const Point finish = api->getFinish();

  const uint dist = start.distanceTo(finish);
  std::vector<Point> pathPoint;

  for(uint i = 0; i <= dist; ++i) {
    pathPoint.push_back(
        hexLerp(start, finish, 1.0 / static_cast<double>(dist) * i));
  }

  std::vector<Direction::T> path;
  for(size_t i = 0; i < pathPoint.size() - 1; ++i) {
    Point diff = pathPoint[i + 1] - pathPoint[i];

    if(diff == Point{1, -1}) {
      path.push_back(Direction::T::eNorth);
    } else if(diff == Point{1, 0}) {
      path.push_back(Direction::T::eNorthEast);
    } else if(diff == Point{0, 1}) {
      path.push_back(Direction::T::eSouthEast);
    } else if(diff == Point{-1, 1}) {
      path.push_back(Direction::T::eSouth);
    } else if(diff == Point{-1, 0}) {
      path.push_back(Direction::T::eSouthWest);
    } else if(diff == Point{0, -1}) {
      path.push_back(Direction::T::eNorthWest);
    }
  }

  return path;
}
