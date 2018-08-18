#include "rallyMap.h"
#include <cmath>
#include <stdexcept>

//-----------------------------------------------------------------------------
// Direction
//-----------------------------------------------------------------------------

// Reverses the given `Direction`.
// Ex. North -> South.
Direction reverseDirection(Direction dir) {
    switch(dir) {
        case Direction::North:
            return Direction::South;
        case Direction::NorthEast:
            return Direction::SouthWest;
        case Direction::SouthEast:
            return Direction::NorthWest;
        case Direction::South:
            return Direction::North;
        case Direction::SouthWest:
            return Direction::NorthEast;
        case Direction::NorthWest:
            return Direction::SouthEast;
        // Just in case.
        default:
            return dir;
    }
}

// Rotates the given `Direction` left/counterclockwise.
// Ex. North -> NorthWest.
Direction rotateDirectionLeft(Direction dir) {
    switch(dir) {
        case Direction::North:
            return Direction::NorthWest;
        case Direction::NorthWest:
            return Direction::SouthWest;
        case Direction::SouthWest:
            return Direction::South;
        case Direction::South:
            return Direction::SouthEast;
        case Direction::SouthEast:
            return Direction::NorthEast;
        case Direction::NorthEast:
            return Direction::North;
        // Just in case.
        default:
            return dir;
    }
}

// Rotates the given `Direction` right/clockwise.
// Ex. North -> NorthEast.
Direction rotateDirectionRight(Direction dir) {
    switch(dir) {
        case Direction::North:
            return Direction::NorthEast;
        case Direction::NorthEast:
            return Direction::SouthEast;
        case Direction::SouthEast:
            return Direction::South;
        case Direction::South:
            return Direction::SouthWest;
        case Direction::SouthWest:
            return Direction::NorthWest;
        case Direction::NorthWest:
            return Direction::North;
        // Just in case.
        default:
            return dir;
    }
}

//-----------------------------------------------------------------------------
// Point
//-----------------------------------------------------------------------------

// Defaults to (0,0)
Point::Point() : x(0), y(0){};

Point::Point(int x, int y) : x(x), y(y){};

Point::Point(const Point& other) : x(other.x), y(other.y){};

void Point::set(int newX, int newY) {
    x = newX;
    y = newY;
}

uint Point::distanceTo(const Point& other) const {
    return abs(this->x - other.y) + abs(this->x + this->y - other.x - other.y) +
           abs(this->y - other.y);
}

bool Point::operator==(const Point& other) const {
    return (this->x == other.x) && (this->y == other.y);
}

bool Point::operator!=(const Point& other) const {
    return (this->x != other.x) || (this->y != other.y);
}

Point& Point::operator=(const Point& other) {
    this->x = other.x;
    this->y = other.y;

    return *this;
}

//-----------------------------------------------------------------------------
// RallyMap
//-----------------------------------------------------------------------------

uint RallyMap::getHeight() const {
    return height;
}

uint RallyMap::getWidth() const {
    return width;
}

Point RallyMap::getStart() const {
    return start;
}

// If the given Point is the same as the finish or out of bounds this
// throws an exception.
void RallyMap::setStart(Point newStart) {
    if(newStart.x < 0 || static_cast<uint>(newStart.x) >= width || newStart.y < 0 ||
       static_cast<uint>(newStart.y) >= height) {
        throw std::range_error("invalid position");
    }

    if(newStart == finish) {
        throw std::invalid_argument("start cannot be the same as finish");
    }

    start = newStart;
}

Point RallyMap::getFinish() const {
    return finish;
}

// If the given Point is the same as the start or out of bounds this
// throws an exception.
void RallyMap::setFinish(Point newFinish) {
    if(newFinish.x < 0 || static_cast<uint>(newFinish.x) >= width || newFinish.y < 0 ||
       static_cast<uint>(newFinish.y) >= height) {
        throw std::range_error("invalid position");
    }

    if(newFinish == start) {
        throw std::invalid_argument("finish cannot be the same as start");
    }

    finish = newFinish;
}

// Sets the start and finish to random Points.
void RallyMap::randomizeRace() {
    start.set(rand() % width, rand() % height);

    finish = start;

    // This is theoretically an infinite loop. With the 2x2 minimum map size
    // this should never be a problem.
    while(finish.x == start.x && finish.y == start.y) {
        finish.set(rand() % width, rand() % height);
    }
}

// Throws an exception if the position is out of bounds.
uint RallyMap::getRoughness(Point pos) const {
    if(pos.x < 0 || static_cast<uint>(pos.x) > width || pos.y < 0 ||
       static_cast<uint>(pos.y) > height) {
        throw std::range_error("invalid position");
    }

    return roughness[pos.y][pos.x];
}

// If the value given is 0, then the roughness is set to a random value.
// Values above the max roughness are set to the max.
// Throws an exception if the position is out of bounds.
void RallyMap::setRoughness(Point pos, uint newRoughness) {
    if(pos.x < 0 || static_cast<uint>(pos.x) > width || pos.y < 0 ||
       static_cast<uint>(pos.y) > height) {
        throw std::range_error("invalid position");
    }

    if(newRoughness > MAX_ROUGHNESS) {
        roughness[pos.y][pos.x] = MAX_ROUGHNESS;
    } else if(newRoughness == 0) {
        roughness[pos.y][pos.x] = rand() % MAX_ROUGHNESS + 1;
    } else {
        roughness[pos.y][pos.x] = newRoughness;
    }
}

// Randomizes the roughness of the entire map.
void RallyMap::randomizeRoughness() {
    for(uint y = 0; y < height; ++y) {
        for(uint x = 0; x < width; ++x) {
            roughness[y][x] = rand() % MAX_ROUGHNESS + 1;
        }
    }
}

std::vector<std::vector<uint>> RallyMap::getAllRoughness() const {
    return roughness;
}

// The map is resized and the roughness is set based on the given template.
// The roughness is set the same way as `setRoughness` so 0 values are
// replaced with random values, and values above the max are set to the max.
//
// Throws an exception if either of the template's dimensions are smaller
// than two or if the template is jagged.
void RallyMap::setMap(Point start,
                      Point finish,
                      const std::vector<std::vector<uint>>& mapTemplate) {
    height = mapTemplate.size();
    width = mapTemplate[0].size();

    if(height < 2 || width < 2) {
        throw std::invalid_argument("the template is too small");
    }

    for(uint i = 0; i < height; ++i) {
        if(mapTemplate[i].size() != width) {
            throw std::invalid_argument("the template cannot be jagged");
        }
    }

    setStart(start);
    setFinish(finish);

    roughness = mapTemplate;

    // Set random values and clamp top of range
    for(uint y = 0; y < height; ++y) {
        for(uint x = 0; x < width; ++x) {
            if(roughness[y][x] == 0) {
                roughness[y][x] = rand() % MAX_ROUGHNESS + 1;
            } else if(roughness[y][x] > MAX_ROUGHNESS) {
                roughness[y][x] = MAX_ROUGHNESS;
            }
        }
    }
}

// Creates a random map with the given dimensions.
//
// Throws an exception if either of the template's dimensions are smaller
// than two.
RallyMap::RallyMap(uint width, uint height) : start(Point(-1, -1)), finish(Point(-1, -1)) {
    if(width < 2 || height < 2) {
        throw std::invalid_argument("map dimensions too small");
    }

    this->height = height;
    this->width = width;

    for(uint y = 0; y < height; ++y) {
        roughness.push_back(std::vector<uint>(width, 1));
    }

    randomizeRoughness();
    randomizeRace();
}

// Creates a map from the given template. This works the same way as calling
// `setMap`.
//
// Throws an exception if either of the template's dimensions are smaller
// than two or if the template is jagged.
RallyMap::RallyMap(Point startPos,
                   Point finishPos,
                   const std::vector<std::vector<uint>>& mapTemplate)
    : start(Point(-1, -1)), finish(Point(-1, -1)) {
    setMap(startPos, finishPos, mapTemplate);
}

// Copy constructor
RallyMap::RallyMap(const RallyMap& other)
    : width(other.width),
      height(other.height),
      start(other.start),
      finish(other.finish),
      roughness(other.roughness) {}

// Calculates where the given path ends, how long to get there, and if it
// ends on the finish.
std::tuple<Point, uint, bool> RallyMap::analyzePath(const std::vector<Direction>& path) const {
    auto pos = start;
    uint time = 0;

    for(auto dir : path) {
        time += getMoveCost(pos, dir);
        pos = getDestination(pos, dir);
    }

    return std::make_tuple(pos, time, pos == finish);
}

// Determines where the given path ends.
Point RallyMap::calculatePathEnd(const std::vector<Direction>& path) const {
    auto pos = start;

    for(auto dir : path) {
        pos = getDestination(pos, dir);
    }

    return pos;
}

// Calculates the time it takes to move in the given directions.
uint RallyMap::calculatePathCost(const std::vector<Direction>& path) const {
    auto pos = start;
    uint time = 0;

    for(auto dir : path) {
        time += getMoveCost(pos, dir);
        pos = getDestination(pos, dir);
    }

    return time;
}

// Creates a list of all the points surrounding the given one, and the
// direction to that point.
std::vector<std::pair<Point, Direction>> RallyMap::getNeighbors(Point pos) const {
    std::vector<std::pair<Point, Direction>> neighborList;

    for(auto dir : DIRECTIONS) {
        auto neighbor = getDestination(pos, dir);

        if(neighbor != pos) {
            neighborList.push_back(std::make_pair(pos, dir));
        }
    }

    return neighborList;
}

// Determines the cost of moving in a given direction. If the move goes out
// of bounds the agent returns to their starting position. This is not the
// same as a no-op, and costs twice the roughness of the starting position.
uint RallyMap::getMoveCost(Point pos, Direction dir) const {
    auto there = getDestination(pos, dir);

    uint roughHere = roughness[pos.y][pos.x];
    uint roughThere = roughness[there.y][there.x];

    if(pos == start || pos == finish) {
        roughHere = 1;
    }

    if(there == start || there == finish) {
        roughThere = 1;
    }

    return roughHere + roughThere;
}

// Determines what Point is arrived at from moving in a given
// direction. In the case of moving out of bounds, the original Point
// is returned.
Point RallyMap::getDestination(Point pos, Direction dir) const {
    switch(dir) {
        // North       x+1, y-1
        case Direction::North:
            if(static_cast<uint>(pos.x + 1) < width && pos.y > 0) {
                pos.x += 1;
                pos.y -= 1;
            }
            break;
        // NorthEast   x+1
        case Direction::NorthEast:
            if(static_cast<uint>(pos.x + 1) < width) {
                pos.x += 1;
            }
            break;
        // SouthEast   y+1
        case Direction::SouthEast:
            if(static_cast<uint>(pos.y + 1) < height) {
                pos.y += 1;
            }
            break;
        // South       x-1, y+1
        case Direction::South:
            if(pos.x > 0 && static_cast<uint>(pos.y + 1) < height) {
                pos.x -= 1;
                pos.y += 1;
            }
            break;
        // SouthWest   x-1
        case Direction::SouthWest:
            if(pos.x > 0) {
                pos.x -= 1;
            }
            break;
        // NorthWest   y-1
        case Direction::NorthWest:
            if(pos.y > 0) {
                pos.y -= 1;
            }
            break;
    }

    return pos;
}

std::string RallyMap::toString() const {
    std::string out = "";

    // Add top indicator for the start.
    for(int x = 0; x < start.x * 2 + start.y; ++x) {
        out += " ";
    }
    out += "|\n";

    for(uint y = 0; y < height; ++y) {
        // Add front spacing
        for(uint space = 0; space < y; ++space) {
            out += " ";
        }

        // Add roughness value.
        for(uint x = 0; x < width; ++x) {
            if(static_cast<uint>(start.x) == x && static_cast<uint>(start.y) == y) {
                out += "* ";
            } else if(static_cast<uint>(finish.x) == x && static_cast<uint>(finish.y) == y) {
                out += "& ";
            } else {
                out += std::to_string(roughness[y][x]) + " ";
            }
        }

        out += "\n";
    }

    // Add bottom indicator for the finish.
    for(int x = 0; x < finish.x * 2 + finish.y; ++x) {
        out += " ";
    }
    out += "|\n";

    return out;
}

RallyMap& RallyMap::operator=(const RallyMap& other) {
    this->width = other.width;
    this->height = other.height;
    this->roughness = other.roughness;
    this->start = other.start;
    this->finish = other.finish;

    return *this;
}

std::ostream& operator<<(std::ostream& os, const RallyMap& map) {
    return os << map.toString();
}
