#include "rallyAgent.h"

uint MapInterface::getHeight() const {
    return map->getHeight();
}

uint MapInterface::getWidth() const {
    return map->getWidth();
}

Point MapInterface::getStart() const {
    return map->getStart();
}

Point MapInterface::getFinish() const {
    return map->getFinish();
}

uint MapInterface::getMapLooks() const {
    return mapLooks;
}

MapInterface::MapInterface(const RallyMap* map) : map(map), mapLooks(0) {}

MapInterface::MapInterface(const MapInterface& other) : map(other.map), mapLooks(0) {}

// Creates a list of all the points surrounding the given one, and the
// direction to that point.
std::vector<std::pair<Point, Direction>> MapInterface::getNeighbors(Point pos) const {
    return map->getNeighbors(pos);
}

// Determines the cost of moving in a given direction. If the move goes out
// of bounds the agent returns to their starting position. This is not the
// same as a no-op, and costs twice the roughness of the starting position.
uint MapInterface::getMoveCost(Point pos, Direction dir) {
    mapLooks += 1;
    return map->getMoveCost(pos, dir);
}

// Determines what Point is arrived at from moving in a given
// direction. In the case of moving out of bounds, the original Point
// is returned.
Point MapInterface::getDestination(Point pos, Direction dir) const {
    return map->getDestination(pos, dir);
}
