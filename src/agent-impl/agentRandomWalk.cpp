#include "agent/agent-impl.h"

using Rally::MapInterface;
using Rally::Point;

namespace {
constexpr uint MAX_STEPS = 50;
}

// This agent forgot to mark the finish on their map, and wanders around trying
// to find it.
REGISTER_AGENT(RandomWalk)(MapInterface* const api) {
  Point currentPos = api->getStart();
  std::vector<Direction::T> path;

  for(uint i = 0; currentPos != api->getFinish() && i < MAX_STEPS; ++i) {
    auto nearby = api->getNeighbors(currentPos);
    Direction::T dir = nearby.at(rand() % nearby.size()).second;
    path.push_back(dir);
    currentPos = api->getDestination(currentPos, dir);
  }

  return path;
}
