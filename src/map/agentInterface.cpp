#include "rallyAgent.h"

uint AgentInterface::getHeight() const {
    return map->getHeight();
}

uint AgentInterface::getWidth() const {
    return map->getWidth();
}

Point AgentInterface::getStart() const {
    return map->getStart();
}

Point AgentInterface::getFinish() const {
    return map->getFinish();
}

uint AgentInterface::getMapLooks() const {
    return mapLooks;
}

AgentInterface::AgentInterface(RallyMap* map) : map(map), mapLooks(0) {}

AgentInterface::AgentInterface(const AgentInterface& other) : map(other.map), mapLooks(0) {}

// Creates a list of all the points surrounding the given one, and the
// direction to that point.
std::vector<std::pair<Point, Direction>> AgentInterface::getNeighbors(Point pos) const {
    return map->getNeighbors(pos);
}

// Determines the cost of moving in a given direction. If the move goes out
// of bounds the agent returns to their starting position. This is not the
// same as a no-op, and costs twice the roughness of the starting position.
uint AgentInterface::getMoveCost(Point pos, Direction dir) {
    mapLooks += 1;
    return map->getMoveCost(pos, dir);
}

// Determines what Point is arrived at from moving in a given
// direction. In the case of moving out of bounds, the original Point
// is returned.
Point AgentInterface::getDestination(Point pos, Direction dir) const {
    return map->getDestination(pos, dir);
}
