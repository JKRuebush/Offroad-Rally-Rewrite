#ifndef RALLYAGENT_H
#define RALLYAGENT_H

#include "mapInterface.h"
#include "rallyMap.h"

class AgentBase {
   public:
    virtual const char* getName() const = 0;

    // This is the function called to run the Agent on a RallyMap
    virtual std::vector<Direction> RunAgent(MapInterface* const api) = 0;

    virtual ~AgentBase() {}
};

class AgentFactoryBase {
   protected:
    AgentFactoryBase() {}

   public:
    virtual AgentBase* CreateTest() = 0;

    virtual ~AgentFactoryBase() {}
};

template <class AgentClass>
class AgentFactory : public AgentFactoryBase {
   public:
    virtual AgentBase* CreateTest() { return new AgentClass; }
};

class AgentManager {
    std::vector<AgentFactoryBase*> agentFactories;

   public:
    static AgentManager* GetInstance() {
        static AgentManager manager;
        return &manager;
    }

    AgentFactoryBase* registerAgent(AgentFactoryBase* fact) {
        agentFactories.push_back(fact);
        return fact;
    }

    std::vector<AgentBase*> makeAgents() const {
        std::vector<AgentBase*> agents;

        for(auto factory : agentFactories) {
            agents.push_back(factory->CreateTest());
        }

        return agents;
    }
};

#define MAKE_AGENT_NAME(name) Agent_##name
#define REGISTER_AGENT(agentName)                                         \
    class MAKE_AGENT_NAME(agentName) : public AgentBase {                 \
        static AgentFactoryBase* const factory;                           \
        const char* name = #agentName;                                    \
                                                                          \
       public:                                                            \
        const char* getName() const { return name; }                      \
                                                                          \
        MAKE_AGENT_NAME(agentName)() {}                                   \
                                                                          \
        virtual std::vector<Direction> RunAgent(MapInterface* const api); \
    };                                                                    \
                                                                          \
    AgentFactoryBase* const MAKE_AGENT_NAME(agentName)::factory =         \
        AgentManager::GetInstance()->registerAgent(                       \
            new AgentFactory<MAKE_AGENT_NAME(agentName)>());              \
    std::vector<Direction> MAKE_AGENT_NAME(agentName)::RunAgent

#endif /* RALLYAGENT_H */
