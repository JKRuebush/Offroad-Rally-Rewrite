#ifndef AGENT_AGENT_MANAGER_H_
#define AGENT_AGENT_MANAGER_H_

#include <memory>
#include <vector>

#include "agent/agent-wrapper.h"
#include "agent/rally-agent.h"

namespace Rally {

// The `AgentManager` is a singleton where all `AgentFactories` are registed. It
// can then use the factories to make the `AgentWrapper` instances that are used
// in the Rally.
class AgentManager {
  std::vector<std::shared_ptr<AgentFactoryBase>> agentFactories;

  AgentManager() = default;

 public:
  // The singleton is retrieved or created if it doesn't already exist.
  static AgentManager* GetInstance();

  // Registers the givent factory, and returns the given pointer.
  std::shared_ptr<AgentFactoryBase> registerAgent(
      std::shared_ptr<AgentFactoryBase> fact);

  // Creates an vector with a single instance of each agent in a wrapper.
  void makeAgents(std::vector<AgentWrapper>& agents) const;
};

}  // namespace Rally

#endif /* AGENT_AGENT_MANAGER_H_ */
