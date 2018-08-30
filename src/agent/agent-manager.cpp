#include "agent/agent-manager.h"

namespace Rally {

AgentManager* AgentManager::GetInstance() {
  static AgentManager manager;
  return &manager;
}

std::shared_ptr<AgentFactoryBase> AgentManager::registerAgent(
    std::shared_ptr<AgentFactoryBase> fact) {
  agentFactories.push_back(fact);
  return fact;
}

void AgentManager::makeAgents(std::vector<AgentWrapper>& agents) const {
  for(const auto& factory : agentFactories) {
    agents.push_back(
        AgentWrapper(std::unique_ptr<AgentBase>(factory->CreateTest())));
  }
}

}  // namespace Rally
