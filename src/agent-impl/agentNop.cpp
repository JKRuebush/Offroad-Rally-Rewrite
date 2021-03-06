#include "agent/agent-impl.h"

using Rally::MapInterface;

// This agent is a no-op, and mostly serves as a test to show everything else
// is working.
REGISTER_AGENT(Nop)(MapInterface* const api) {
  return std::vector<Direction::T>{};
}
