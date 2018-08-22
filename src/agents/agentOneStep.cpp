#include "rallyAgent.h"

// This agent is lazy, and only does the race if the finish is nearby.
REGISTER_AGENT(OneStep)(MapInterface* const api) {
    auto nearby = api->getNeighbors(api->getStart());

    for(auto hex : nearby) {
        if(hex.first == api->getFinish()) {
            return std::vector<Direction>{hex.second};
        }
    }

    return std::vector<Direction>{};
}
