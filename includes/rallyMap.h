#ifndef RALLYMAP_H
#define RALLYMAP_H

#include <string>
#include <tuple>
#include <vector>

// The bottom of this range is 1. As of right now only 1-9 is supported.
const uint MAX_ROUGHNESS = 9;

enum class Direction {
    North,     // x+1, y-1
    NorthEast, // x+1
    SouthEast, // y+1
    South,     // x-1, y+1
    SouthWest, // x-1
    NorthWest  // y-1
};

const Direction DIRECTIONS[] = {
    Direction::North, Direction::NorthEast, Direction::SouthEast, Direction::South, 
    Direction::SouthWest, Direction::NorthWest
};

// Reverses the given `Direction`.
// Ex. North -> South.
Direction reverseDirection(Direction dir);
// Rotates the given `Direction` left/counterclockwise.
// Ex. North -> NorthWest.
Direction rotateDirectionLeft(Direction dir);
// Rotates the given `Direction` right/clockwise.
// Ex. North -> NorthEast.
Direction rotateDirectionRight(Direction dir);

class Point {
public:
    int x;
    int y;

    // Defaults to (0,0)
    Point();
    Point(int x, int y);
    Point(const Point& other);

    void set(int newX, int newY);

    uint distanceTo(const Point& other) const;

    Point operator+(const Point& other) const;
    Point operator-(const Point& other) const;

    bool operator==(const Point& other) const;
    bool operator!=(const Point& other) const;

    Point& operator=(const Point& other);
};

// The `RallyMap` represents the hex map that the rally takes place on. For
// simple storage and displaying the underlying structure is a rhombus. Each hex
// has a roughness score. The time it takes to move from one hex to another is
// the sum of the roughness of the two hexes. If an agent tries to move off of 
// the hex grid they are directed back onto the hex they started on.
//
// Note: The start and finish are always treated as if they have a roughness of
// one.
class RallyMap {
    size_t width;
    size_t height;

    Point start;
    Point finish;

    std::vector<std::vector<uint> > roughness;
public:
    size_t getHeight() const;
    size_t getWidth() const;

    Point getStart() const;
    // If the given Point is the same as the finish or out of bounds this
    // throws an exception.
    void setStart(Point newStart);

    Point getFinish() const;
    // If the given Point is the same as the start or out of bounds this
    // throws an exception.
    void setFinish(Point newFinish);

    // Sets the start and finish to random Points.
    void randomizeRace();

    // Throws an exception if the position is out of bounds.
    uint getRoughness(Point pos) const;
    // If the value given is 0, then the roughness is set to a random value. 
    // Values above the max roughness are set to the max.
    // Throws an exception if the position is out of bounds.
    void setRoughness(Point pos, uint newRoughness); 
    // Randomizes the roughness of the entire map.
    void randomizeRoughness();

    std::vector<std::vector<uint>> getAllRoughness() const;
    // The map is resized and the roughness is set based on the given template.
    // The roughness is set the same way as `setRoughness` so 0 values are
    // replaced with random values, and values above the max are set to the max.
    //
    // Throws an exception if either of the template's dimensions are smaller 
    // than two or if the template is jagged.
    void setMap(Point start, Point finish, const std::vector<std::vector<uint> >& mapTemplate);

    // Creates a random map with the given dimensions.
    //
    // Throws an exception if either of the template's dimensions are smaller 
    // than two.
    RallyMap(uint width, uint height);
    // Creates a map from the given template. This works the same way as calling
    // `setMap`.
    //
    // Throws an exception if either of the template's dimensions are smaller 
    // than two or if the template is jagged.
    RallyMap(Point startPos, Point finishPos, const std::vector<std::vector<uint> >& mapTemplate);
    // Copy constructor
    RallyMap(const RallyMap& other);

    // Calculates where the given path ends, how long to get there, and if it
    // ends on the finish.
    std::tuple<Point, uint, bool> analyzePath(const std::vector<Direction>& path) const;
    // Determines where the given path ends.
    Point calculatePathEnd(const std::vector<Direction>& path) const;
    // Calculates the time it takes to move in the given directions.
    uint calculatePathTime(const std::vector<Direction>& path) const;

    // Creates a list of all the points surrounding the given one, and the
    // direction to that point. 
    std::vector<std::pair<Point, Direction> > getNeighbors(Point pos) const;

    // Determines the cost of moving in a given direction. If the move goes out
    // of bounds the agent returns to their starting position. This is not the
    // same as a no-op, and costs twice the roughness of the starting position.
    uint getMoveCost(Point pos, Direction dir) const;

    // Determines what Point is arrived at from moving in a given
    // direction. In the case of moving out of bounds, the original Point
    // is returned.
    Point getDestination(Point pos, Direction dir) const;
 
    std::string toString() const;

    RallyMap& operator=(const RallyMap& other);

    friend std::ostream& operator<<(std::ostream &os, const RallyMap& map);
};

#endif /* RALLYMAP_H */
