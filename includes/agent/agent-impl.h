#ifndef AGENT_AGENT_IMPL_H_
#define AGENT_AGENT_IMPL_H_

#include <memory>
#include <vector>

#include "agent/agent-manager.h"
#include "agent/rally-agent.h"

#define MAKE_AGENT_NAME(name) Agent_##name

// This macro is what automatically registers an agent.
//
// This works by using the code following the macro as the definition of the
// `RunAgent` function in an automatically created wrapper class. A factory is
// created that knows how to make this wrapper, and the factory is registered in
// the global `AgentManager` instance.
//
// The `(MapInterface* const api)` part of `RunAgent` is left exposed so
// the availability of the `MapInterface` is obvious, and so that it may be
// named as desired.
#define REGISTER_AGENT(agentName)                                      \
  class MAKE_AGENT_NAME(agentName) : public Rally::AgentBase {         \
    static std::shared_ptr<Rally::AgentFactoryBase> const factory;     \
    const char* name = #agentName;                                     \
                                                                       \
   public:                                                             \
    const char* getName() const { return name; }                       \
                                                                       \
    MAKE_AGENT_NAME(agentName)() {}                                    \
                                                                       \
    virtual std::vector<Direction::T> RunAgent(                        \
        Rally::MapInterface* const api);                               \
  };                                                                   \
                                                                       \
  std::shared_ptr<Rally::AgentFactoryBase> const MAKE_AGENT_NAME(      \
      agentName)::factory =                                            \
      Rally::AgentManager::GetInstance()->registerAgent(               \
          std::shared_ptr<Rally::AgentFactoryBase>(                    \
              new Rally::AgentFactory<MAKE_AGENT_NAME(agentName)>())); \
  std::vector<Direction::T> MAKE_AGENT_NAME(agentName)::RunAgent

#endif /* AGENT_AGENT_IMPL_H_ */
