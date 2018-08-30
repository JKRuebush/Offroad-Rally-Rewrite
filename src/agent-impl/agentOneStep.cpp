#include "agent/agent-impl.h"

using Rally::MapInterface;

// This agent is lazy, and only does the race if the finish is nearby.
REGISTER_AGENT(OneStep)(MapInterface* const api) {
  const auto nearby = api->getNeighbors(api->getStart());

  for(const auto& hex : nearby) {
    if(hex.first == api->getFinish()) {
      return std::vector<Direction::T>{hex.second};
    }
  }

  return std::vector<Direction::T>{};
}
