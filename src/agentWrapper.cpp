
#include "rallyAgent.h"

void AgentWrapper::addRace(const RallyMap* rally) {
    MapInterface api(rally);

    path = agent->RunAgent(&api);
    mapLooks = api.getMapLooks();
    std::tie(pathCost, finishedRace) = rally->analyzePath(path);

    totalMapLooks += mapLooks;
    totalPathCost += pathCost;

    if(finishedRace) {
        racesFinished += 1;
    }
}

bool AgentWrapper::orderLastRace(const AgentWrapper& a, const AgentWrapper& b) {
    if(a.finishedRace != b.finishedRace) {
        return a.finishedRace && !b.finishedRace;
    } else if(a.pathCost == b.pathCost) {
        return a.mapLooks < b.mapLooks;
    } else {
        return a.pathCost < b.pathCost;
    }
}

bool AgentWrapper::orderAllRace(const AgentWrapper& a, const AgentWrapper& b) {
    if(a.racesFinished != b.racesFinished) {
        return a.racesFinished > b.racesFinished;
    } else if(a.totalPathCost == b.totalPathCost) {
        return a.totalMapLooks < b.totalMapLooks;
    } else {
        return a.totalPathCost < b.totalPathCost;
    }
}
