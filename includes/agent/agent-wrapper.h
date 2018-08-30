#ifndef AGENT_AGENT_WRAPPER_H_
#define AGENT_AGENT_WRAPPER_H_

#include <memory>

#include "agent/rally-agent.h"
#include "map/hex-direction.h"

namespace Rally {

// AgentWrapper collects statistics on Agent implementations, and manages
// interactions with the agent.
class AgentWrapper {
  std::unique_ptr<AgentBase> agent;

 public:
  // Single race statistics.
  std::vector<Direction::T> path;
  uint mapLooks;
  uint pathCost;
  bool finishedRace;

  // Overall statistics.
  uint totalMapLooks;
  uint totalPathCost;
  uint racesFinished;

  const char* getName() const;

  explicit AgentWrapper(std::unique_ptr<AgentBase> agent);

  void addRace(const RallyMap& rally);

  // This can be passed to functions like `std::sort` to sort agents by how
  // agents performed in the last race.
  static bool operatorOrderLastRace(const AgentWrapper& a,
                                    const AgentWrapper& b);

  // This can be passed to functions like `std::sort` to sort agents by how
  // agents performed overall.
  static bool operatorOrderAllRace(const AgentWrapper& a,
                                   const AgentWrapper& b);
};

}  // namespace Rally

#endif /* AGENT_AGENT_WRAPPER_H_ */
