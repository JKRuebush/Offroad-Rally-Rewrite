#ifndef RALLY_AGENT_H_
#define RALLY_AGENT_H_

#include "map/hex-direction.h"
#include "map/map-interface.h"
#include "map/rally-map.h"

namespace Rally {

class AgentBase {
 public:
  virtual const char* getName() const = 0;

  // This is the function called to run the Agent on a RallyMap
  virtual std::vector<Direction::T> RunAgent(MapInterface* const api) = 0;

  virtual ~AgentBase() {}
};

class AgentFactoryBase {
 protected:
  AgentFactoryBase() {}

 public:
  virtual AgentBase* CreateTest() const = 0;

  virtual ~AgentFactoryBase() {}
};

template <class AgentClass>
class AgentFactory : public AgentFactoryBase {
 public:
  virtual AgentBase* CreateTest() const { return new AgentClass; }
};

}  // namespace Rally

#endif /* RALLY_AGENT_H_ */
