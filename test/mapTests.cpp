#include <gtest/gtest.h>

#include "rallyMap.h"

namespace {
std::vector<std::vector<uint> > testTemplate{
    {8, 8, 8, 8, 8, 1, 8, 8, 8, 8, 8, 1, 8, 8, 8, 8, 8, 1, 8, 8, 8, 8, 8},
    {1, 1, 8, 1, 1, 1, 8, 1, 1, 1, 1, 1, 8, 1, 1, 1, 1, 1, 1, 1, 8, 1, 1},
    {1, 1, 8, 1, 1, 1, 8, 8, 8, 8, 8, 1, 8, 8, 8, 8, 8, 1, 1, 1, 8, 1, 1},
    {1, 1, 8, 1, 1, 1, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 8, 1, 1, 1, 8, 1, 1},
    {1, 1, 8, 1, 1, 1, 8, 8, 8, 8, 8, 1, 8, 8, 8, 8, 8, 1, 1, 1, 8, 1, 1}};

std::vector<std::vector<uint> > randomSquareemplate{{0, 0, 0, 0, 0},
                                                    {0, 0, 0, 0, 0},
                                                    {0, 0, 0, 0, 0},
                                                    {0, 0, 0, 0, 0},
                                                    {0, 0, 0, 0, 0}};

std::vector<std::vector<uint> > largeValueTemplate{{9000, 9000, 9000, 9000, 9000},
                                                   {9000, 9000, 9000, 9000, 9000},
                                                   {9000, 9000, 9000, 9000, 9000},
                                                   {9000, 9000, 9000, 9000, 9000},
                                                   {9000, 9000, 9000, 9000, 9000}};

std::vector<Direction> diagonalPath{
    Direction::NorthEast, Direction::SouthEast, Direction::NorthEast, Direction::SouthEast,
    Direction::NorthEast, Direction::SouthEast, Direction::NorthEast, Direction::SouthEast};

std::vector<Direction> spiralPath{
    Direction::NorthEast, Direction::NorthEast, Direction::NorthEast, Direction::NorthEast,
    Direction::SouthEast, Direction::SouthEast, Direction::SouthEast, Direction::SouthEast,
    Direction::SouthWest, Direction::SouthWest, Direction::SouthWest, Direction::SouthWest,
    Direction::NorthWest, Direction::NorthWest, Direction::NorthWest, Direction::NorthEast,
    Direction::NorthEast, Direction::NorthEast, Direction::SouthEast, Direction::SouthEast,
    Direction::SouthWest, Direction::SouthWest, Direction::NorthWest, Direction::NorthEast};

std::vector<Direction> allDirectionsPath{Direction::NorthEast, Direction::South,
                                         Direction::SouthEast, Direction::NorthWest,
                                         Direction::North,     Direction::SouthWest};

std::vector<Direction> followBottomToFinishPath{
    Direction::SouthEast, Direction::SouthEast, Direction::SouthEast, Direction::SouthEast,
    Direction::NorthEast, Direction::NorthEast, Direction::NorthEast, Direction::NorthEast,
    Direction::NorthEast, Direction::NorthEast, Direction::NorthEast, Direction::NorthEast,
    Direction::NorthEast, Direction::NorthEast, Direction::NorthEast, Direction::NorthEast,
    Direction::NorthEast, Direction::NorthEast, Direction::NorthEast, Direction::NorthEast,
    Direction::NorthEast, Direction::NorthEast, Direction::NorthEast, Direction::NorthEast,
    Direction::NorthEast, Direction::NorthEast};

std::vector<Direction> shorterPath{
    Direction::SouthEast, Direction::SouthEast, Direction::SouthEast, Direction::SouthEast,
    Direction::NorthEast, Direction::NorthEast, Direction::NorthEast, Direction::NorthEast,
    Direction::NorthEast, Direction::NorthEast, Direction::North,     Direction::NorthEast,
    Direction::NorthEast, Direction::NorthEast, Direction::NorthEast, Direction::NorthEast,
    Direction::NorthEast, Direction::NorthEast, Direction::NorthEast, Direction::NorthEast,
    Direction::NorthEast, Direction::NorthEast, Direction::NorthEast, Direction::NorthEast,
    Direction::NorthEast, Direction::NorthEast, Direction::SouthEast};
}  // namespace

TEST(RallyMap, ConstructorBySize) {
    // Ensure minimum map size is properly enforced.
    EXPECT_ANY_THROW(RallyMap(2, 1));
    EXPECT_ANY_THROW(RallyMap(1, 2));
    EXPECT_ANY_THROW(RallyMap(1, 1));
    EXPECT_ANY_THROW(RallyMap(0, 0));

    EXPECT_NO_THROW(RallyMap(2, 2));

    // Ensure internals properly setup.
    RallyMap rectangle(5, 15);

    ASSERT_EQ(rectangle.getWidth(), 5);
    ASSERT_EQ(rectangle.getHeight(), 15);

    RallyMap testMapA(9, 9);
    RallyMap testMapB(9, 9);

    ASSERT_EQ(testMapA.getWidth(), 9);
    ASSERT_EQ(testMapA.getHeight(), 9);
    ASSERT_EQ(testMapB.getWidth(), 9);
    ASSERT_EQ(testMapB.getHeight(), 9);

    // This technically could be true even when properly implemented. However,
    // the probability of a false positive is incredibly low; something like a
    // 5.086×10^-76% chance.
    ASSERT_NE(testMapA.getAllRoughness(), testMapB.getAllRoughness());

    // The chance of the start and finish being the same by chance is much
    // higher. For a similar percentage chance as the roughness being the same
    // the start and finish are compared 41 times to ensure they are different
    // at least once.
    bool differentStart = false;
    bool differentFinish = false;

    for(uint i = 0; i < 41 && (!differentStart || !differentFinish); ++i) {
        RallyMap diffTest(9, 9);

        if(diffTest.getStart() != testMapA.getStart()) {
            differentStart = true;
        }

        if(diffTest.getFinish() != testMapA.getFinish()) {
            differentFinish = true;
        }
    }

    ASSERT_TRUE(differentStart && differentFinish);

    // Ensure all roughness values are in the proper range.
    RallyMap rangeTest(16, 16);
    auto rough = rangeTest.getAllRoughness();

    bool outOfRange = false;

    for(size_t y = 0; y < rangeTest.getWidth(); ++y) {
        for(size_t x = 0; x < rangeTest.getHeight(); ++x) {
            if(rough[y][x] < 1 || rough[y][x] > MAX_ROUGHNESS) {
                outOfRange = true;
                goto rangeTestEnd;
            }
        }
    }
rangeTestEnd:

    ASSERT_FALSE(outOfRange);
}

TEST(RallyMap, ConstructorFromTemplate) {
    // Ensure map size and point requirements are respected.
    EXPECT_ANY_THROW(RallyMap(Point(0, 0), Point(0, 0), std::vector<std::vector<uint> >{{}}));
    EXPECT_ANY_THROW(RallyMap(Point(-1, 0), Point(0, 0), testTemplate));
    EXPECT_ANY_THROW(RallyMap(Point(0, -1), Point(0, 0), testTemplate));
    EXPECT_ANY_THROW(RallyMap(Point(0, 0), Point(-1, 0), testTemplate));
    EXPECT_ANY_THROW(RallyMap(Point(0, 0), Point(0, -1), testTemplate));
    EXPECT_ANY_THROW(RallyMap(Point(9000, 0), Point(0, 0), testTemplate));
    EXPECT_ANY_THROW(RallyMap(Point(0, 9000), Point(0, 0), testTemplate));
    EXPECT_ANY_THROW(RallyMap(Point(0, 0), Point(9000, 0), testTemplate));
    EXPECT_ANY_THROW(RallyMap(Point(0, 0), Point(0, 9000), testTemplate));

    // Check that everything is properly set internally.
    Point start(4, 2);
    Point finish(0, 0);
    RallyMap testRally(start, finish, testTemplate);

    ASSERT_EQ(testRally.getWidth(), testTemplate[0].size());
    ASSERT_EQ(testRally.getHeight(), testTemplate.size());
    ASSERT_EQ(testRally.getAllRoughness(), testTemplate);
    ASSERT_EQ(testRally.getStart(), start);
    ASSERT_EQ(testRally.getFinish(), finish);

    // Ensure 0 is converted to a random value in templates.
    RallyMap randomTest(start, finish, randomSquareemplate);

    ASSERT_NE(testRally.getAllRoughness(), randomSquareemplate);

    auto randomRough = randomTest.getAllRoughness();
    bool randomValueInRange = true;

    for(size_t y = 0; y < randomTest.getWidth(); ++y) {
        for(size_t x = 0; x < randomTest.getHeight(); ++x) {
            if(randomRough[y][x] < 1 || randomRough[y][x] > MAX_ROUGHNESS) {
                randomValueInRange = false;
                goto randomRangeTestEnd;
            }
        }
    }
randomRangeTestEnd:

    ASSERT_TRUE(randomValueInRange);

    // Ensure large values are reduced to the max value.
    RallyMap largeValueTest(start, finish, largeValueTemplate);

    ASSERT_NE(testRally.getAllRoughness(), largeValueTemplate);

    auto largeRough = largeValueTest.getAllRoughness();
    bool largeValueTruncated = true;

    for(size_t y = 0; y < largeValueTest.getWidth(); ++y) {
        for(size_t x = 0; x < largeValueTest.getHeight(); ++x) {
            if(largeRough[y][x] != MAX_ROUGHNESS) {
                largeValueTruncated = false;
                goto largeValueTestEnd;
            }
        }
    }
largeValueTestEnd:

    ASSERT_TRUE(largeValueTruncated);
}

TEST(RallyMap, ConstructorCopy) {
    RallyMap original(9, 9);
    RallyMap copy(original);

    ASSERT_EQ(original.getAllRoughness(), copy.getAllRoughness());
    ASSERT_EQ(original.getStart(), copy.getStart());
    ASSERT_EQ(original.getFinish(), copy.getFinish());
}

TEST(RallyMap, Randomization) {
    RallyMap original(9, 9);
    RallyMap copy(original);

    copy.randomizeRoughness();

    ASSERT_NE(original.getAllRoughness(), copy.getAllRoughness());
    ASSERT_EQ(original.getStart(), copy.getStart());
    ASSERT_EQ(original.getFinish(), copy.getFinish());

    // Randomizing to the same thing isn't incredibly unlikely, but doing it
    // multiple times is unlikely.
    Point originalStart = copy.getStart();
    Point originalFinish = copy.getFinish();

    bool differentStart = false;
    bool differentFinish = false;

    for(uint i = 0; i < 41 && (!differentStart || !differentFinish); ++i) {
        copy.randomizeRace();

        if(copy.getStart() != originalStart) {
            differentStart = true;
        }

        if(copy.getFinish() != originalFinish) {
            differentFinish = true;
        }
    }

    ASSERT_TRUE(differentStart && differentFinish);
}

TEST(RallyMap, Destination) {
    Point topLeft(0, 0);
    Point top(2, 0);
    Point topRight(4, 0);
    Point left(0, 2);
    Point middle(2, 2);
    Point right(4, 2);
    Point bottomLeft(0, 4);
    Point bottom(2, 4);
    Point bottomRight(4, 4);

    RallyMap testRally(topLeft, bottomRight, randomSquareemplate);

    // Only NorthEast and SouthEast shouldn't bounce back.
    ASSERT_EQ(testRally.getDestination(topLeft, Direction::North), topLeft);
    ASSERT_EQ(testRally.getDestination(topLeft, Direction::NorthEast), Point(1, 0));
    ASSERT_EQ(testRally.getDestination(topLeft, Direction::SouthEast), Point(0, 1));
    ASSERT_EQ(testRally.getDestination(topLeft, Direction::South), topLeft);
    ASSERT_EQ(testRally.getDestination(topLeft, Direction::SouthWest), topLeft);
    ASSERT_EQ(testRally.getDestination(topLeft, Direction::NorthWest), topLeft);

    // North and NorthWest should bounce back.
    ASSERT_EQ(testRally.getDestination(top, Direction::North), top);
    ASSERT_EQ(testRally.getDestination(top, Direction::NorthEast), Point(3, 0));
    ASSERT_EQ(testRally.getDestination(top, Direction::SouthEast), Point(2, 1));
    ASSERT_EQ(testRally.getDestination(top, Direction::South), Point(1, 1));
    ASSERT_EQ(testRally.getDestination(top, Direction::SouthWest), Point(1, 0));
    ASSERT_EQ(testRally.getDestination(top, Direction::NorthWest), top);

    // The North* directions should bounce back.
    ASSERT_EQ(testRally.getDestination(topRight, Direction::North), topRight);
    ASSERT_EQ(testRally.getDestination(topRight, Direction::NorthEast), topRight);
    ASSERT_EQ(testRally.getDestination(topRight, Direction::SouthEast), Point(4, 1));
    ASSERT_EQ(testRally.getDestination(topRight, Direction::South), Point(3, 1));
    ASSERT_EQ(testRally.getDestination(topRight, Direction::SouthWest), Point(3, 0));
    ASSERT_EQ(testRally.getDestination(topRight, Direction::NorthWest), topRight);

    // South and SouthWest bounce back
    ASSERT_EQ(testRally.getDestination(left, Direction::North), Point(1, 1));
    ASSERT_EQ(testRally.getDestination(left, Direction::NorthEast), Point(1, 2));
    ASSERT_EQ(testRally.getDestination(left, Direction::SouthEast), Point(0, 3));
    ASSERT_EQ(testRally.getDestination(left, Direction::South), left);
    ASSERT_EQ(testRally.getDestination(left, Direction::SouthWest), left);
    ASSERT_EQ(testRally.getDestination(left, Direction::NorthWest), Point(0, 1));

    // No bounceback in the middle of the map.
    ASSERT_EQ(testRally.getDestination(middle, Direction::North), Point(3, 1));
    ASSERT_EQ(testRally.getDestination(middle, Direction::NorthEast), Point(3, 2));
    ASSERT_EQ(testRally.getDestination(middle, Direction::SouthEast), Point(2, 3));
    ASSERT_EQ(testRally.getDestination(middle, Direction::South), Point(1, 3));
    ASSERT_EQ(testRally.getDestination(middle, Direction::SouthWest), Point(1, 2));
    ASSERT_EQ(testRally.getDestination(middle, Direction::NorthWest), Point(2, 1));

    // North and NorthEast bounce back.
    ASSERT_EQ(testRally.getDestination(right, Direction::North), right);
    ASSERT_EQ(testRally.getDestination(right, Direction::NorthEast), right);
    ASSERT_EQ(testRally.getDestination(right, Direction::SouthEast), Point(4, 3));
    ASSERT_EQ(testRally.getDestination(right, Direction::South), Point(3, 3));
    ASSERT_EQ(testRally.getDestination(right, Direction::SouthWest), Point(3, 2));
    ASSERT_EQ(testRally.getDestination(right, Direction::NorthWest), Point(4, 1));

    // The South* directions bounce back
    ASSERT_EQ(testRally.getDestination(bottomLeft, Direction::North), Point(1, 3));
    ASSERT_EQ(testRally.getDestination(bottomLeft, Direction::NorthEast), Point(1, 4));
    ASSERT_EQ(testRally.getDestination(bottomLeft, Direction::SouthEast), bottomLeft);
    ASSERT_EQ(testRally.getDestination(bottomLeft, Direction::South), bottomLeft);
    ASSERT_EQ(testRally.getDestination(bottomLeft, Direction::SouthWest), bottomLeft);
    ASSERT_EQ(testRally.getDestination(bottomLeft, Direction::NorthWest), Point(0, 3));

    // South and SouthEast bounce back
    ASSERT_EQ(testRally.getDestination(bottom, Direction::North), Point(3, 3));
    ASSERT_EQ(testRally.getDestination(bottom, Direction::NorthEast), Point(3, 4));
    ASSERT_EQ(testRally.getDestination(bottom, Direction::SouthEast), bottom);
    ASSERT_EQ(testRally.getDestination(bottom, Direction::South), bottom);
    ASSERT_EQ(testRally.getDestination(bottom, Direction::SouthWest), Point(1, 4));
    ASSERT_EQ(testRally.getDestination(bottom, Direction::NorthWest), Point(2, 3));

    // Only NorthWest and SouthWest shouldn't bounce back.
    ASSERT_EQ(testRally.getDestination(bottomRight, Direction::North), bottomRight);
    ASSERT_EQ(testRally.getDestination(bottomRight, Direction::NorthEast), bottomRight);
    ASSERT_EQ(testRally.getDestination(bottomRight, Direction::SouthEast), bottomRight);
    ASSERT_EQ(testRally.getDestination(bottomRight, Direction::South), bottomRight);
    ASSERT_EQ(testRally.getDestination(bottomRight, Direction::SouthWest), Point(3, 4));
    ASSERT_EQ(testRally.getDestination(bottomRight, Direction::NorthWest), Point(4, 3));
}

TEST(RallyMap, MoveCost) {
    RallyMap costTest(Point(3, 0), Point(3, 2),
                      std::vector<std::vector<uint> >{{1, 2, 3, 7}, {4, 5, 6, 8}, {7, 8, 9, 9}});

    // Simple cases
    ASSERT_EQ(costTest.getMoveCost(Point(1, 1), Direction::North), 8);
    ASSERT_EQ(costTest.getMoveCost(Point(1, 1), Direction::NorthEast), 11);
    ASSERT_EQ(costTest.getMoveCost(Point(1, 1), Direction::SouthEast), 13);
    ASSERT_EQ(costTest.getMoveCost(Point(1, 1), Direction::South), 12);
    ASSERT_EQ(costTest.getMoveCost(Point(1, 1), Direction::SouthWest), 9);
    ASSERT_EQ(costTest.getMoveCost(Point(1, 1), Direction::NorthWest), 7);

    // Corner case including bounceback
    ASSERT_EQ(costTest.getMoveCost(Point(0, 2), Direction::North), 12);
    ASSERT_EQ(costTest.getMoveCost(Point(0, 2), Direction::NorthEast), 15);
    ASSERT_EQ(costTest.getMoveCost(Point(0, 2), Direction::SouthEast), 14);
    ASSERT_EQ(costTest.getMoveCost(Point(0, 2), Direction::South), 14);
    ASSERT_EQ(costTest.getMoveCost(Point(0, 2), Direction::SouthWest), 14);
    ASSERT_EQ(costTest.getMoveCost(Point(0, 2), Direction::NorthWest), 11);

    // To and from start including bounceback
    ASSERT_EQ(costTest.getMoveCost(Point(3, 0), Direction::North), 2);
    ASSERT_EQ(costTest.getMoveCost(Point(3, 0), Direction::NorthEast), 2);
    ASSERT_EQ(costTest.getMoveCost(Point(3, 0), Direction::SouthEast), 9);
    ASSERT_EQ(costTest.getMoveCost(Point(3, 0), Direction::South), 7);
    ASSERT_EQ(costTest.getMoveCost(Point(3, 0), Direction::SouthWest), 4);
    ASSERT_EQ(costTest.getMoveCost(Point(3, 0), Direction::NorthWest), 2);

    ASSERT_EQ(costTest.getMoveCost(Point(2, 0), Direction::NorthEast), 4);
    ASSERT_EQ(costTest.getMoveCost(Point(2, 1), Direction::North), 7);
    ASSERT_EQ(costTest.getMoveCost(Point(3, 1), Direction::NorthWest), 9);

    // To and from finish including bounceback
    ASSERT_EQ(costTest.getMoveCost(Point(3, 2), Direction::North), 2);
    ASSERT_EQ(costTest.getMoveCost(Point(3, 2), Direction::NorthEast), 2);
    ASSERT_EQ(costTest.getMoveCost(Point(3, 2), Direction::SouthEast), 2);
    ASSERT_EQ(costTest.getMoveCost(Point(3, 2), Direction::South), 2);
    ASSERT_EQ(costTest.getMoveCost(Point(3, 2), Direction::SouthWest), 10);
    ASSERT_EQ(costTest.getMoveCost(Point(3, 2), Direction::NorthWest), 9);

    ASSERT_EQ(costTest.getMoveCost(Point(3, 1), Direction::SouthEast), 9);
    ASSERT_EQ(costTest.getMoveCost(Point(2, 2), Direction::NorthEast), 10);
}

TEST(RallyMap, PathEnd) {
    RallyMap pathTest(Point(0, 0), Point(22, 4), testTemplate);

    // Not moving is valid.
    ASSERT_EQ(pathTest.calculatePathEnd(std::vector<Direction>{}), Point(0, 0));

    // Single move.
    ASSERT_EQ(pathTest.calculatePathEnd(std::vector<Direction>{Direction::North}), Point(0, 0));
    ASSERT_EQ(pathTest.calculatePathEnd(std::vector<Direction>{Direction::NorthEast}), Point(1, 0));
    ASSERT_EQ(pathTest.calculatePathEnd(std::vector<Direction>{Direction::SouthEast}), Point(0, 1));
    ASSERT_EQ(pathTest.calculatePathEnd(std::vector<Direction>{Direction::South}), Point(0, 0));
    ASSERT_EQ(pathTest.calculatePathEnd(std::vector<Direction>{Direction::SouthWest}), Point(0, 0));
    ASSERT_EQ(pathTest.calculatePathEnd(std::vector<Direction>{Direction::NorthWest}), Point(0, 0));

    // Arbitrary movement.
    ASSERT_EQ(pathTest.calculatePathEnd(diagonalPath), Point(4, 4));
    ASSERT_EQ(pathTest.calculatePathEnd(spiralPath), Point(2, 2));
    ASSERT_EQ(pathTest.calculatePathEnd(allDirectionsPath), Point(0, 0));
    ASSERT_EQ(pathTest.calculatePathEnd(followBottomToFinishPath), Point(22, 4));
    ASSERT_EQ(pathTest.calculatePathEnd(shorterPath), Point(22, 4));
}

TEST(RallyMap, PathCost) {
    RallyMap pathTest(Point(0, 0), Point(22, 4), testTemplate);

    // Not moving is valid.
    ASSERT_EQ(pathTest.calculatePathCost(std::vector<Direction>{}), 0);

    // Single move.
    ASSERT_EQ(pathTest.calculatePathCost(std::vector<Direction>{Direction::North}), 2);
    ASSERT_EQ(pathTest.calculatePathCost(std::vector<Direction>{Direction::NorthEast}), 9);
    ASSERT_EQ(pathTest.calculatePathCost(std::vector<Direction>{Direction::SouthEast}), 2);
    ASSERT_EQ(pathTest.calculatePathCost(std::vector<Direction>{Direction::South}), 2);
    ASSERT_EQ(pathTest.calculatePathCost(std::vector<Direction>{Direction::SouthWest}), 2);
    ASSERT_EQ(pathTest.calculatePathCost(std::vector<Direction>{Direction::NorthWest}), 2);

    // Arbitrary movement.
    ASSERT_EQ(pathTest.calculatePathCost(diagonalPath), 58);
    ASSERT_EQ(pathTest.calculatePathCost(spiralPath), 153);
    ASSERT_EQ(pathTest.calculatePathCost(allDirectionsPath), 40);
    ASSERT_EQ(pathTest.calculatePathCost(followBottomToFinishPath), 220);
    ASSERT_EQ(pathTest.calculatePathCost(shorterPath), 110);
}
