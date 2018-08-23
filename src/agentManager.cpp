#include "rallyAgent.h"

AgentManager* AgentManager::GetInstance() {
    static AgentManager manager;
    return &manager;
}

AgentFactoryBase* AgentManager::registerAgent(AgentFactoryBase* fact) {
    agentFactories.push_back(fact);
    return fact;
}

void AgentManager::makeAgents(std::vector<AgentWrapper>& agents) const {
    for(auto factory : agentFactories) {
        agents.push_back(AgentWrapper(std::unique_ptr<AgentBase>(factory->CreateTest())));
    }
}
