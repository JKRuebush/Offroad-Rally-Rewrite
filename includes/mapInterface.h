#ifndef MAPINTERFACE_H
#define MAPINTERFACE_H

#include "rallyMap.h"

class MapInterface {
    RallyMap* map;
    uint mapLooks;

   public:
    uint getHeight() const;
    uint getWidth() const;

    Point getStart() const;
    Point getFinish() const;

    uint getMapLooks() const;

    MapInterface(RallyMap* map);
    MapInterface(const MapInterface& other);

    // Creates a list of all the points surrounding the given one, and the
    // direction to that point.
    std::vector<std::pair<Point, Direction>> getNeighbors(Point pos) const;

    // Determines the cost of moving in a given direction. If the move goes out
    // of bounds the agent returns to their starting position. This is not the
    // same as a no-op, and costs twice the roughness of the starting position.
    uint getMoveCost(Point pos, Direction dir);

    // Determines what Point is arrived at from moving in a given
    // direction. In the case of moving out of bounds, the original Point
    // is returned.
    Point getDestination(Point pos, Direction dir) const;
};

#endif /* MAPINTERFACE_H */
