#ifndef MAP_RALLY_MAP_H_
#define MAP_RALLY_MAP_H_

#include <cmath>
#include <string>
#include <tuple>
#include <vector>

#include "map/hex-direction.h"

typedef unsigned int uint;

namespace Rally {

// The bottom of this range is 1. As of right now only 1-9 is supported.
constexpr uint kMaxRoughness = 9;

struct Point {
  int x;
  int y;

  inline uint distanceTo(const Point& other) const {
    return (std::abs(this->x - other.x) +
            std::abs(this->x + this->y - other.x - other.y) +
            std::abs(this->y - other.y)) /
           2;
  }

  // Lower bounds are inclusive, and upper bounds are exclusive.
  inline bool inBounds(int minX, int minY, int maxX, int maxY) const {
    return x >= minX && x < maxX && y >= minY && y < maxY;
  }

  inline bool inBounds(const Point& bottomLeft, const Point& topRight) const {
    return x >= bottomLeft.x && x < topRight.x && y >= bottomLeft.y &&
           y < topRight.y;
  }

  inline Point operator+(const Point& rhs) const {
    return {this->x + rhs.x, this->y + rhs.y};
  }

  inline Point operator-(const Point& rhs) const {
    return {this->x - rhs.x, this->y - rhs.y};
  }

  inline bool operator==(const Point& rhs) const {
    return this->x == rhs.x && this->y == rhs.y;
  }

  inline bool operator!=(const Point& rhs) const { return !operator==(rhs); }

  // This os defined purely for sorting. A dictionary ordering is used with `x`
  // being compared first, and if equal `y` being compared.
  inline bool operator<(const Point& rhs) const {
    return (this->x < rhs.x) || (this->x == rhs.x && this->y < rhs.y);
  }

  // Declared so there are no unexpectedly missing operators.
  inline bool operator>(const Point& rhs) const { return rhs.operator<(*this); }
  inline bool operator<=(const Point& rhs) const { return !operator>(rhs); }
  inline bool operator>=(const Point& rhs) const { return !operator<(rhs); }
};

// The `RallyMap` represents the hex map that the rally takes place on. For
// simple storage and displaying the underlying structure is a rhombus. Each hex
// has a roughness score. The time it takes to move from one hex to another is
// the sum of the roughness of the two hexes. If an agent tries to move off of
// the hex grid they are directed back onto the hex they started on.
//
// Note: The start and finish are always treated as if they have a roughness of
// one.
class RallyMap {
  uint width;
  uint height;

  Point start;
  Point finish;

  std::vector<std::vector<uint>> roughness;

 public:
  inline uint getHeight() const { return height; }
  inline uint getWidth() const { return width; }

  inline Point getStart() const { return start; }
  inline Point getFinish() const { return finish; }

  // This throws an exception if the start and finish are the same or if
  // either point is outside of the map.
  void setEndPoints(Point nStart, Point nFinish);

  void randomizeEndPoints();

  // Throws an exception if the position is out of bounds.
  uint getRoughness(Point pos) const;
  // If the value given is 0, then the roughness is set to a random value.
  // Values above the max roughness are set to the max.
  // Throws an exception if the position is out of bounds.
  void setRoughness(Point pos, uint newRoughness);
  // Randomizes the roughness of the entire map.
  void randomizeRoughness();

  std::vector<std::vector<uint>> getAllRoughness() const;
  // The map is resized and the roughness is set based on the given template.
  // The roughness is set the same way as `setRoughness` so 0 values are
  // replaced with random values, and values above the max are set to the max.
  //
  // Throws an exception if either of the template's dimensions are smaller
  // than two or if the template is jagged.
  void setMap(Point start,
              Point finish,
              const std::vector<std::vector<uint>>& mapTemplate);

  // Creates a random map with the given dimensions.
  //
  // Throws an exception if either of the template's dimensions are smaller
  // than two.
  RallyMap(uint width, uint height);
  // Creates a map from the given template. This works the same way as calling
  // `setMap`.
  //
  // Throws an exception if either of the template's dimensions are smaller
  // than two or if the template is jagged.
  RallyMap(Point startPos,
           Point finishPos,
           const std::vector<std::vector<uint>>& mapTemplate);
  // Copy constructor
  RallyMap(const RallyMap& other);

  // Calculates the cost of the path, and if it ends on the finish.
  std::pair<uint, bool> analyzePath(
      const std::vector<Direction::T>& path) const;
  // Determines where the given path ends.
  Point calculatePathEnd(const std::vector<Direction::T>& path) const;
  // Calculates the cost it takes to move in the given directions.
  uint calculatePathCost(const std::vector<Direction::T>& path) const;

  // Creates a list of all the points surrounding the given one, and the
  // direction to that point.
  std::vector<std::pair<Point, Direction::T>> getNeighbors(Point pos) const;

  // Determines the cost of moving in a given direction. If the move goes out
  // of bounds the agent returns to their starting position. This is not the
  // same as a no-op, and costs twice the roughness of the starting position.
  uint getMoveCost(Point pos, Direction::T dir) const;

  // Determines what Point is arrived at from moving in a given
  // direction. In the case of moving out of bounds, the original Point
  // is returned.
  Point getDestination(Point pos, Direction::T dir) const;

  std::string toString() const;

  RallyMap& operator=(const RallyMap& other);

  friend std::ostream& operator<<(std::ostream& os, const RallyMap& map);
};

}  // namespace Rally

#endif /* MAP_RALLY_MAP_H_ */
