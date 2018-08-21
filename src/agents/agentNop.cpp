#include "rallyAgent.h"

REGISTER_AGENT(Nop)(const MapInterface* api) {
    return std::vector<Direction>{};
}
