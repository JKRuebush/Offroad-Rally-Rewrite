#ifndef MAP_HEX_DIRECTION_H_
#define MAP_HEX_DIRECTION_H_

#include <array>

namespace Direction {

enum class T : char {
  eNorth,      // x+1, y-1
  eNorthEast,  // x+1
  eSouthEast,  // y+1
  eSouth,      // x-1, y+1
  eSouthWest,  // x-1
  eNorthWest,  // y-1
  eNone
};

constexpr std::array<T, 6> kAllMoveDirections{T::eNorth,     T::eNorthEast,
                                              T::eSouthEast, T::eSouth,
                                              T::eSouthWest, T::eNorthWest};

// Reverses the given `Direction`.
// Ex. North -> South.
T reverse(T dir);
// Rotates the given `Direction` left/counterclockwise.
// Ex. North -> NorthWest.
T rotateLeft(T dir);
// Rotates the given `Direction` right/clockwise.
// Ex. North -> NorthEast.
T rotateRight(T dir);

}  // namespace Direction

#endif /* MAP_HEX_DIRECTION_H_ */
