#include <cmath>
#include <cstdlib>
#include <stdexcept>

#include "map/rally-map.h"

namespace Rally {

// This throws an exception if the start and finish are the same or if
// either point is outside of the map.
void RallyMap::setEndPoints(Point nStart, Point nFinish) {
  if(nStart == nFinish) {
    throw std::invalid_argument("rally end points cannot be the same");
  }

  if(!nStart.inBounds(0, 0, width, height)) {
    throw std::range_error("start point out of bounds");
  }

  if(!nFinish.inBounds(0, 0, width, height)) {
    throw std::range_error("finish point out of bounds");
  }

  start = nStart;
  finish = nFinish;
}

// Sets the start and finish to random Points.
void RallyMap::randomizeEndPoints() {
  start = {static_cast<int>(rand() % width), static_cast<int>(rand() % height)};

  finish = start;

  // This is theoretically an infinite loop. With the 2x2 minimum map size
  // this should never be a problem.
  while(finish.x == start.x && finish.y == start.y) {
    finish = {static_cast<int>(rand() % width),
              static_cast<int>(rand() % height)};
  }
}

// Throws an exception if the position is out of bounds.
uint RallyMap::getRoughness(Point pos) const {
  if(!pos.inBounds(0, 0, width, height)) {
    throw std::range_error("invalid position");
  }

  return roughness[pos.y][pos.x];
}

// If the value given is 0, then the roughness is set to a random value.
// Values above the max roughness are set to the max.
// Throws an exception if the position is out of bounds.
void RallyMap::setRoughness(Point pos, uint newRoughness) {
  if(!pos.inBounds(0, 0, width, height)) {
    throw std::range_error("invalid position");
  }

  if(newRoughness > kMaxRoughness) {
    roughness[pos.y][pos.x] = kMaxRoughness;
  } else if(newRoughness == 0) {
    roughness[pos.y][pos.x] = rand() % kMaxRoughness + 1;
  } else {
    roughness[pos.y][pos.x] = newRoughness;
  }
}

// Randomizes the roughness of the entire map.
void RallyMap::randomizeRoughness() {
  for(uint y = 0; y < height; ++y) {
    for(uint x = 0; x < width; ++x) {
      roughness[y][x] = rand() % kMaxRoughness + 1;
    }
  }
}

std::vector<std::vector<uint>> RallyMap::getAllRoughness() const {
  return roughness;
}

// The map is resized and the roughness is set based on the given template.
// The roughness is set the same way as `setRoughness` so 0 values are
// replaced with random values, and values above the max are set to the max.
//
// Throws an exception if either of the template's dimensions are smaller
// than two or if the template is jagged.
void RallyMap::setMap(Point start,
                      Point finish,
                      const std::vector<std::vector<uint>>& mapTemplate) {
  height = mapTemplate.size();
  width = mapTemplate[0].size();

  if(height < 2 || width < 2) {
    throw std::invalid_argument("the template is too small");
  }

  for(uint i = 0; i < height; ++i) {
    if(mapTemplate[i].size() != width) {
      throw std::invalid_argument("the template cannot be jagged");
    }
  }

  setEndPoints(start, finish);

  roughness = mapTemplate;

  // Set random values and clamp top of range
  for(uint y = 0; y < height; ++y) {
    for(uint x = 0; x < width; ++x) {
      if(roughness[y][x] == 0) {
        roughness[y][x] = rand() % kMaxRoughness + 1;
      } else if(roughness[y][x] > kMaxRoughness) {
        roughness[y][x] = kMaxRoughness;
      }
    }
  }
}

// Creates a random map with the given dimensions.
//
// Throws an exception if either of the template's dimensions are smaller
// than two.
RallyMap::RallyMap(uint width, uint height) {
  if(width < 2 || height < 2) {
    throw std::invalid_argument("map dimensions too small");
  }

  this->height = height;
  this->width = width;

  for(uint y = 0; y < height; ++y) {
    roughness.push_back(std::vector<uint>(width, 1));
  }

  randomizeRoughness();
  randomizeEndPoints();
}

// Creates a map from the given template. This works the same way as calling
// `setMap`.
//
// Throws an exception if either of the template's dimensions are smaller
// than two or if the template is jagged.
RallyMap::RallyMap(Point startPos,
                   Point finishPos,
                   const std::vector<std::vector<uint>>& mapTemplate)
    : start({-1, -1}), finish({-1, -1}) {
  setMap(startPos, finishPos, mapTemplate);
}

// Copy constructor
RallyMap::RallyMap(const RallyMap& other)
    : width(other.width),
      height(other.height),
      start(other.start),
      finish(other.finish),
      roughness(other.roughness) {}

// Calculates the cost of the path, and if it ends on the finish.
std::pair<uint, bool> RallyMap::analyzePath(
    const std::vector<Direction::T>& path) const {
  auto pos = start;
  uint time = 0;

  for(const auto& dir : path) {
    time += getMoveCost(pos, dir);
    pos = getDestination(pos, dir);
  }

  return std::pair<uint, bool>(time, pos == finish);
}

// Determines where the given path ends.
Point RallyMap::calculatePathEnd(const std::vector<Direction::T>& path) const {
  auto pos = start;

  for(const auto& dir : path) {
    pos = getDestination(pos, dir);
  }

  return pos;
}

// Calculates the time it takes to move in the given directions.
uint RallyMap::calculatePathCost(const std::vector<Direction::T>& path) const {
  auto pos = start;
  uint time = 0;

  for(const auto& dir : path) {
    time += getMoveCost(pos, dir);
    pos = getDestination(pos, dir);
  }

  return time;
}

// Creates a list of all the points surrounding the given one, and the
// direction to that point.
std::vector<std::pair<Point, Direction::T>> RallyMap::getNeighbors(
    Point pos) const {
  std::vector<std::pair<Point, Direction::T>> neighborList;

  for(const auto& dir : Direction::kAllMoveDirections) {
    auto neighbor = getDestination(pos, dir);

    if(neighbor != pos) {
      neighborList.push_back(std::pair<Point, Direction::T>(neighbor, dir));
    }
  }

  return neighborList;
}

// Determines the cost of moving in a given direction. If the move goes out
// of bounds the agent returns to their starting position. This is not the
// same as a no-op, and costs twice the roughness of the starting position.
uint RallyMap::getMoveCost(Point pos, Direction::T dir) const {
  auto there = getDestination(pos, dir);

  uint roughHere = roughness[pos.y][pos.x];
  uint roughThere = roughness[there.y][there.x];

  if(pos == start || pos == finish) {
    roughHere = 1;
  }

  if(there == start || there == finish) {
    roughThere = 1;
  }

  return roughHere + roughThere;
}

// Determines what Point is arrived at from moving in a given
// direction. In the case of moving out of bounds, the original Point
// is returned.
Point RallyMap::getDestination(Point pos, Direction::T dir) const {
  switch(dir) {
    // North       x+1, y-1
    case Direction::T::eNorth:
      if(static_cast<uint>(pos.x + 1) < width && pos.y > 0) {
        pos.x += 1;
        pos.y -= 1;
      }
      break;
    // NorthEast   x+1
    case Direction::T::eNorthEast:
      if(static_cast<uint>(pos.x + 1) < width) {
        pos.x += 1;
      }
      break;
    // SouthEast   y+1
    case Direction::T::eSouthEast:
      if(static_cast<uint>(pos.y + 1) < height) {
        pos.y += 1;
      }
      break;
    // South       x-1, y+1
    case Direction::T::eSouth:
      if(pos.x > 0 && static_cast<uint>(pos.y + 1) < height) {
        pos.x -= 1;
        pos.y += 1;
      }
      break;
    // SouthWest   x-1
    case Direction::T::eSouthWest:
      if(pos.x > 0) {
        pos.x -= 1;
      }
      break;
    // NorthWest   y-1
    case Direction::T::eNorthWest:
      if(pos.y > 0) {
        pos.y -= 1;
      }
      break;
    case Direction::T::eNone:
      break;
  }

  return pos;
}

std::string RallyMap::toString() const {
  std::string out = "";

  // Add top indicator for the start.
  for(int x = 0; x < start.x * 2 + start.y; ++x) {
    out += " ";
  }
  out += "|\n";

  for(uint y = 0; y < height; ++y) {
    // Add front spacing
    for(uint space = 0; space < y; ++space) {
      out += " ";
    }

    // Add roughness value.
    for(uint x = 0; x < width; ++x) {
      if(static_cast<uint>(start.x) == x && static_cast<uint>(start.y) == y) {
        out += "*";
      } else if(static_cast<uint>(finish.x) == x &&
                static_cast<uint>(finish.y) == y) {
        out += "&";
      } else {
        out += std::to_string(roughness[y][x]);
      }

      if(x + 1 != width) {
        out += " ";
      }
    }

    out += "\n";
  }

  // Add bottom indicator for the finish.
  for(int x = 0; x < finish.x * 2 + finish.y; ++x) {
    out += " ";
  }
  out += "|\n";

  return out;
}

RallyMap& RallyMap::operator=(const RallyMap& other) {
  this->width = other.width;
  this->height = other.height;
  this->roughness = other.roughness;
  this->start = other.start;
  this->finish = other.finish;

  return *this;
}

std::ostream& operator<<(std::ostream& os, const RallyMap& map) {
  return os << map.toString();
}

}  // namespace Rally
