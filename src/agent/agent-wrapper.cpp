
#include "agent/agent-wrapper.h"

namespace Rally {

const char* AgentWrapper::getName() const {
  return agent->getName();
}

AgentWrapper::AgentWrapper(std::unique_ptr<AgentBase> agent)
    : agent(std::move(agent)),
      mapLooks(0),
      pathCost(0),
      finishedRace(false),

      totalMapLooks(0),
      totalPathCost(0),
      racesFinished(0) {}

void AgentWrapper::addRace(const RallyMap& rally) {
  MapInterface api(rally);

  path = agent->RunAgent(&api);
  mapLooks = api.getMapLooks();
  std::tie(pathCost, finishedRace) = rally.analyzePath(path);

  totalMapLooks += mapLooks;
  totalPathCost += pathCost;

  if(finishedRace) {
    racesFinished += 1;
  }
}

bool AgentWrapper::operatorOrderLastRace(const AgentWrapper& a,
                                         const AgentWrapper& b) {
  if(a.finishedRace != b.finishedRace) {
    return a.finishedRace && !b.finishedRace;
  } else if(a.pathCost == b.pathCost) {
    return a.mapLooks < b.mapLooks;
  } else {
    return a.pathCost < b.pathCost;
  }
}

bool AgentWrapper::operatorOrderAllRace(const AgentWrapper& a,
                                        const AgentWrapper& b) {
  if(a.racesFinished != b.racesFinished) {
    return a.racesFinished > b.racesFinished;
  } else if(a.totalPathCost == b.totalPathCost) {
    return a.totalMapLooks < b.totalMapLooks;
  } else {
    return a.totalPathCost < b.totalPathCost;
  }
}

}  // namespace Rally
