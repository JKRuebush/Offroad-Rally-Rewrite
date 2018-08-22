#include <cmath>

#include "rallyAgent.h"

namespace {
Point hexRound(double x, double y) {
    double roundedX = round(x);
    double roundedY = round(y);
    double roundedZ = round(-x - y);

    double deltaX = fabs(roundedX - x);
    double deltaY = fabs(roundedY - y);
    double deltaZ = fabs(roundedZ + x + y);

    // Floating point error shouldn't be an issue here as a large amount of
    // precision isn't needed.
    if(deltaX > deltaY && deltaX > deltaZ) {
        roundedX = -roundedY - roundedZ;
    } else if(deltaY > deltaZ) {
        roundedY = -roundedX - roundedZ;
    }

    return Point(static_cast<int>(roundedX), static_cast<int>(roundedY));
}

inline double lerp(int a, int b, double t) {
    return static_cast<double>(a) + static_cast<double>(b - a) * t;
}

inline Point hexLerp(Point a, Point b, double t) {
    return hexRound(lerp(a.x, b.x, t), lerp(a.y, b.y, t));
}

}  // namespace

// This agent takes the straight path towards the finish line.
REGISTER_AGENT(Crow)(MapInterface* const api) {
    auto start = api->getStart();
    auto finish = api->getFinish();

    uint dist = start.distanceTo(finish);
    std::vector<Point> pathPoint;

    for(uint i = 0; i <= dist; ++i) {
        pathPoint.push_back(hexLerp(start, finish, 1.0 / static_cast<double>(dist) * i));
    }

    std::vector<Direction> path;
    for(size_t i = 0; i < pathPoint.size() - 1; ++i) {
        Point diff = pathPoint[i + 1] - pathPoint[i];

        if(diff == Point(1, -1)) {
            path.push_back(Direction::North);
        } else if(diff == Point(1, 0)) {
            path.push_back(Direction::NorthEast);
        } else if(diff == Point(0, 1)) {
            path.push_back(Direction::SouthEast);
        } else if(diff == Point(-1, 1)) {
            path.push_back(Direction::South);
        } else if(diff == Point(-1, 0)) {
            path.push_back(Direction::SouthWest);
        } else if(diff == Point(0, -1)) {
            path.push_back(Direction::NorthWest);
        }
    }

    return path;
}
