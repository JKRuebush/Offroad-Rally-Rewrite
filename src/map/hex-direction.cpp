
#include "map/hex-direction.h"

namespace Direction {

// Reverses the given `Direction`.
// Ex. North -> South.
T reverse(T dir) {
  switch(dir) {
    case T::eNorth:
      return T::eSouth;
    case T::eNorthEast:
      return T::eSouthWest;
    case T::eSouthEast:
      return T::eNorthWest;
    case T::eSouth:
      return T::eNorth;
    case T::eSouthWest:
      return T::eNorthEast;
    case T::eNorthWest:
      return T::eSouthEast;
    case T::eNone:
      return T::eNone;
    // Just in case.
    default:
      return dir;
  }
}

// Rotates the given `Direction` left/counterclockwise.
// Ex. North -> NorthWest.
T rotateLeft(T dir) {
  switch(dir) {
    case T::eNorth:
      return T::eNorthWest;
    case T::eNorthWest:
      return T::eSouthWest;
    case T::eSouthWest:
      return T::eSouth;
    case T::eSouth:
      return T::eSouthEast;
    case T::eSouthEast:
      return T::eNorthEast;
    case T::eNorthEast:
      return T::eNorth;
    case T::eNone:
      return T::eNone;
  }
}

// Rotates the given `Direction` right/clockwise.
// Ex. North -> NorthEast.
T rotateRight(T dir) {
  switch(dir) {
    case T::eNorth:
      return T::eNorthEast;
    case T::eNorthEast:
      return T::eSouthEast;
    case T::eSouthEast:
      return T::eSouth;
    case T::eSouth:
      return T::eSouthWest;
    case T::eSouthWest:
      return T::eNorthWest;
    case T::eNorthWest:
      return T::eNorth;
    case T::eNone:
      return T::eNone;
  }
}

}  // namespace Direction
