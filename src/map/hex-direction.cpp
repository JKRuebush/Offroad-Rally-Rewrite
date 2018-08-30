
#include "map/hex-direction.h"

namespace Direction {

// Reverses the given `Direction`.
// Ex. North -> South.
Direction::T reverseDirection(Direction::T dir) {
  switch(dir) {
    case Direction::T::eNorth:
      return Direction::T::eSouth;
    case Direction::T::eNorthEast:
      return Direction::T::eSouthWest;
    case Direction::T::eSouthEast:
      return Direction::T::eNorthWest;
    case Direction::T::eSouth:
      return Direction::T::eNorth;
    case Direction::T::eSouthWest:
      return Direction::T::eNorthEast;
    case Direction::T::eNorthWest:
      return Direction::T::eSouthEast;
    case Direction::T::eNone:
      return Direction::T::eNone;
    // Just in case.
    default:
      return dir;
  }
}

// Rotates the given `Direction` left/counterclockwise.
// Ex. North -> NorthWest.
Direction::T rotateDirectionLeft(Direction::T dir) {
  switch(dir) {
    case Direction::T::eNorth:
      return Direction::T::eNorthWest;
    case Direction::T::eNorthWest:
      return Direction::T::eSouthWest;
    case Direction::T::eSouthWest:
      return Direction::T::eSouth;
    case Direction::T::eSouth:
      return Direction::T::eSouthEast;
    case Direction::T::eSouthEast:
      return Direction::T::eNorthEast;
    case Direction::T::eNorthEast:
      return Direction::T::eNorth;
    case Direction::T::eNone:
      return Direction::T::eNone;
    // Just in case.
    default:
      return dir;
  }
}

// Rotates the given `Direction` right/clockwise.
// Ex. North -> NorthEast.
Direction::T rotateDirectionRight(Direction::T dir) {
  switch(dir) {
    case Direction::T::eNorth:
      return Direction::T::eNorthEast;
    case Direction::T::eNorthEast:
      return Direction::T::eSouthEast;
    case Direction::T::eSouthEast:
      return Direction::T::eSouth;
    case Direction::T::eSouth:
      return Direction::T::eSouthWest;
    case Direction::T::eSouthWest:
      return Direction::T::eNorthWest;
    case Direction::T::eNorthWest:
      return Direction::T::eNorth;
    case Direction::T::eNone:
      return Direction::T::eNone;
    // Just in case.
    default:
      return dir;
  }
}

}  // namespace Direction
