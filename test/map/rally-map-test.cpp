#include <gtest/gtest.h>

#include "map/rally-map.h"

using Rally::Point;
using Rally::RallyMap;

namespace {
std::vector<std::vector<uint>> kTestTemplate{
    {8, 8, 8, 8, 8, 1, 8, 8, 8, 8, 8, 1, 8, 8, 8, 8, 8, 1, 8, 8, 8, 8, 8},
    {1, 1, 8, 1, 1, 1, 8, 1, 1, 1, 1, 1, 8, 1, 1, 1, 1, 1, 1, 1, 8, 1, 1},
    {1, 1, 8, 1, 1, 1, 8, 8, 8, 8, 8, 1, 8, 8, 8, 8, 8, 1, 1, 1, 8, 1, 1},
    {1, 1, 8, 1, 1, 1, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 8, 1, 1, 1, 8, 1, 1},
    {1, 1, 8, 1, 1, 1, 8, 8, 8, 8, 8, 1, 8, 8, 8, 8, 8, 1, 1, 1, 8, 1, 1}};

std::vector<std::vector<uint>> kRandomSquareemplate{{0, 0, 0, 0, 0},
                                                    {0, 0, 0, 0, 0},
                                                    {0, 0, 0, 0, 0},
                                                    {0, 0, 0, 0, 0},
                                                    {0, 0, 0, 0, 0}};

std::vector<std::vector<uint>> kLargeValueTemplate{
    {9000, 9000, 9000, 9000, 9000},
    {9000, 9000, 9000, 9000, 9000},
    {9000, 9000, 9000, 9000, 9000},
    {9000, 9000, 9000, 9000, 9000},
    {9000, 9000, 9000, 9000, 9000}};

std::vector<Direction::T> kDiagonalPath{
    Direction::T::eNorthEast, Direction::T::eSouthEast,
    Direction::T::eNorthEast, Direction::T::eSouthEast,
    Direction::T::eNorthEast, Direction::T::eSouthEast,
    Direction::T::eNorthEast, Direction::T::eSouthEast};

std::vector<Direction::T> kSpiralPath{
    Direction::T::eNorthEast, Direction::T::eNorthEast,
    Direction::T::eNorthEast, Direction::T::eNorthEast,
    Direction::T::eSouthEast, Direction::T::eSouthEast,
    Direction::T::eSouthEast, Direction::T::eSouthEast,
    Direction::T::eSouthWest, Direction::T::eSouthWest,
    Direction::T::eSouthWest, Direction::T::eSouthWest,
    Direction::T::eNorthWest, Direction::T::eNorthWest,
    Direction::T::eNorthWest, Direction::T::eNorthEast,
    Direction::T::eNorthEast, Direction::T::eNorthEast,
    Direction::T::eSouthEast, Direction::T::eSouthEast,
    Direction::T::eSouthWest, Direction::T::eSouthWest,
    Direction::T::eNorthWest, Direction::T::eNorthEast};

std::vector<Direction::T> kAllDirectionsPath{
    Direction::T::eNorthEast, Direction::T::eSouth, Direction::T::eSouthEast,
    Direction::T::eNorthWest, Direction::T::eNorth, Direction::T::eSouthWest};

std::vector<Direction::T> kFollowBottomToFinishPath{
    Direction::T::eSouthEast, Direction::T::eSouthEast,
    Direction::T::eSouthEast, Direction::T::eSouthEast,
    Direction::T::eNorthEast, Direction::T::eNorthEast,
    Direction::T::eNorthEast, Direction::T::eNorthEast,
    Direction::T::eNorthEast, Direction::T::eNorthEast,
    Direction::T::eNorthEast, Direction::T::eNorthEast,
    Direction::T::eNorthEast, Direction::T::eNorthEast,
    Direction::T::eNorthEast, Direction::T::eNorthEast,
    Direction::T::eNorthEast, Direction::T::eNorthEast,
    Direction::T::eNorthEast, Direction::T::eNorthEast,
    Direction::T::eNorthEast, Direction::T::eNorthEast,
    Direction::T::eNorthEast, Direction::T::eNorthEast,
    Direction::T::eNorthEast, Direction::T::eNorthEast};

std::vector<Direction::T> kShorterPath{
    Direction::T::eSouthEast, Direction::T::eSouthEast,
    Direction::T::eSouthEast, Direction::T::eSouthEast,
    Direction::T::eNorthEast, Direction::T::eNorthEast,
    Direction::T::eNorthEast, Direction::T::eNorthEast,
    Direction::T::eNorthEast, Direction::T::eNorthEast,
    Direction::T::eNorth,     Direction::T::eNorthEast,
    Direction::T::eNorthEast, Direction::T::eNorthEast,
    Direction::T::eNorthEast, Direction::T::eNorthEast,
    Direction::T::eNorthEast, Direction::T::eNorthEast,
    Direction::T::eNorthEast, Direction::T::eNorthEast,
    Direction::T::eNorthEast, Direction::T::eNorthEast,
    Direction::T::eNorthEast, Direction::T::eNorthEast,
    Direction::T::eNorthEast, Direction::T::eNorthEast,
    Direction::T::eSouthEast};
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

  EXPECT_EQ(rectangle.getWidth(), 5);
  EXPECT_EQ(rectangle.getHeight(), 15);

  RallyMap testMapA(9, 9);

  EXPECT_EQ(testMapA.getWidth(), 9);
  EXPECT_EQ(testMapA.getHeight(), 9);

  // This technically could be true even when properly implemented. However,
  // the probability of a false positive is incredibly low; something like a
  // 5.086×10^-76% chance.
  EXPECT_NE(testMapA.getAllRoughness(), RallyMap(9, 9).getAllRoughness());

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

  EXPECT_TRUE(differentStart && differentFinish);

  // Ensure all roughness values are in the proper range.
  RallyMap rangeTest(16, 16);
  auto rough = rangeTest.getAllRoughness();

  bool outOfRange = false;

  for(size_t y = 0; y < rangeTest.getWidth(); ++y) {
    for(size_t x = 0; x < rangeTest.getHeight(); ++x) {
      if(rough[y][x] < 1 || rough[y][x] > Rally::kMaxRoughness) {
        outOfRange = true;
        goto rangeTestEnd;
      }
    }
  }
rangeTestEnd:

  EXPECT_FALSE(outOfRange);
}

TEST(RallyMap, ConstructorFromTemplate) {
  // Ensure map size and point requirements are respected.
  EXPECT_ANY_THROW(
      RallyMap({0, 0}, {0, 0}, std::vector<std::vector<uint>>{{}}));
  EXPECT_ANY_THROW(RallyMap({-1, 0}, {0, 0}, kTestTemplate));
  EXPECT_ANY_THROW(RallyMap({0, -1}, {0, 0}, kTestTemplate));
  EXPECT_ANY_THROW(RallyMap({0, 0}, {-1, 0}, kTestTemplate));
  EXPECT_ANY_THROW(RallyMap({0, 0}, {0, -1}, kTestTemplate));
  EXPECT_ANY_THROW(RallyMap({9000, 0}, {0, 0}, kTestTemplate));
  EXPECT_ANY_THROW(RallyMap({0, 9000}, {0, 0}, kTestTemplate));
  EXPECT_ANY_THROW(RallyMap({0, 0}, {9000, 0}, kTestTemplate));
  EXPECT_ANY_THROW(RallyMap({0, 0}, {0, 9000}, kTestTemplate));

  // Check that everything is properly set internally.
  Point start{4, 2};
  Point finish{0, 0};
  RallyMap testRally(start, finish, kTestTemplate);

  EXPECT_EQ(testRally.getWidth(), kTestTemplate[0].size());
  EXPECT_EQ(testRally.getHeight(), kTestTemplate.size());
  EXPECT_EQ(testRally.getAllRoughness(), kTestTemplate);
  EXPECT_EQ(testRally.getStart(), start);
  EXPECT_EQ(testRally.getFinish(), finish);

  // Ensure 0 is converted to a random value in templates.
  RallyMap randomTest(start, finish, kRandomSquareemplate);

  EXPECT_NE(testRally.getAllRoughness(), kRandomSquareemplate);

  auto randomRough = randomTest.getAllRoughness();
  bool randomValueInRange = true;

  for(size_t y = 0; y < randomTest.getWidth(); ++y) {
    for(size_t x = 0; x < randomTest.getHeight(); ++x) {
      if(randomRough[y][x] < 1 || randomRough[y][x] > Rally::kMaxRoughness) {
        randomValueInRange = false;
        goto randomRangeTestEnd;
      }
    }
  }
randomRangeTestEnd:

  EXPECT_TRUE(randomValueInRange);

  // Ensure large values are reduced to the max value.
  RallyMap largeValueTest(start, finish, kLargeValueTemplate);

  EXPECT_NE(testRally.getAllRoughness(), kLargeValueTemplate);

  auto largeRough = largeValueTest.getAllRoughness();
  bool largeValueTruncated = true;

  for(size_t y = 0; y < largeValueTest.getWidth(); ++y) {
    for(size_t x = 0; x < largeValueTest.getHeight(); ++x) {
      if(largeRough[y][x] != Rally::kMaxRoughness) {
        largeValueTruncated = false;
        goto largeValueTestEnd;
      }
    }
  }
largeValueTestEnd:

  EXPECT_TRUE(largeValueTruncated);
}

TEST(RallyMap, ConstructorCopy) {
  RallyMap original(9, 9);
  RallyMap copy(original);

  EXPECT_EQ(original.getAllRoughness(), copy.getAllRoughness());
  EXPECT_EQ(original.getStart(), copy.getStart());
  EXPECT_EQ(original.getFinish(), copy.getFinish());
}

TEST(RallyMap, SetMap) {
  RallyMap testRally(3, 3);

  // Ensure map size and point requirements are respected.
  EXPECT_ANY_THROW(
      testRally.setMap({0, 0}, {0, 0}, std::vector<std::vector<uint>>{{}}));
  EXPECT_ANY_THROW(testRally.setMap({-1, 0}, {0, 0}, kTestTemplate));
  EXPECT_ANY_THROW(testRally.setMap({0, -1}, {0, 0}, kTestTemplate));
  EXPECT_ANY_THROW(testRally.setMap({0, 0}, {-1, 0}, kTestTemplate));
  EXPECT_ANY_THROW(testRally.setMap({0, 0}, {0, -1}, kTestTemplate));
  EXPECT_ANY_THROW(testRally.setMap({9000, 0}, {0, 0}, kTestTemplate));
  EXPECT_ANY_THROW(testRally.setMap({0, 9000}, {0, 0}, kTestTemplate));
  EXPECT_ANY_THROW(testRally.setMap({0, 0}, {9000, 0}, kTestTemplate));
  EXPECT_ANY_THROW(testRally.setMap({0, 0}, {0, 9000}, kTestTemplate));

  // Check that everything is properly set internally.
  Point start{4, 2};
  Point finish{0, 0};
  testRally.setMap(start, finish, kTestTemplate);

  EXPECT_EQ(testRally.getWidth(), kTestTemplate[0].size());
  EXPECT_EQ(testRally.getHeight(), kTestTemplate.size());
  EXPECT_EQ(testRally.getAllRoughness(), kTestTemplate);
  EXPECT_EQ(testRally.getStart(), start);
  EXPECT_EQ(testRally.getFinish(), finish);
}

TEST(RallyMap, Randomization) {
  RallyMap original(9, 9);
  RallyMap copy(original);

  copy.randomizeRoughness();

  EXPECT_EQ(original.getStart(), copy.getStart());
  EXPECT_EQ(original.getFinish(), copy.getFinish());
  EXPECT_NE(original.getAllRoughness(), copy.getAllRoughness());

  // Randomizing to the same thing isn't incredibly unlikely, but doing it
  // multiple times is unlikely.
  Point originalStart = copy.getStart();
  Point originalFinish = copy.getFinish();

  bool differentStart = false;
  bool differentFinish = false;

  for(uint i = 0; i < 41 && (!differentStart || !differentFinish); ++i) {
    copy.randomizeEndPoints();

    if(copy.getStart() != originalStart) {
      differentStart = true;
    }

    if(copy.getFinish() != originalFinish) {
      differentFinish = true;
    }
  }

  EXPECT_TRUE(differentStart && differentFinish);
}

TEST(RallyMap, Destination) {
  Point topLeft{0, 0};
  Point top{2, 0};
  Point topRight{4, 0};
  Point left{0, 2};
  Point middle{2, 2};
  Point right{4, 2};
  Point bottomLeft{0, 4};
  Point bottom{2, 4};
  Point bottomRight{4, 4};

  using namespace Direction;

  RallyMap testRally(topLeft, bottomRight, kRandomSquareemplate);

  // Only NorthEast and SouthEast shouldn't bounce back.
  EXPECT_EQ(testRally.getDestination(topLeft, T::eNorth), topLeft);
  EXPECT_EQ(testRally.getDestination(topLeft, T::eNorthEast), (Point{1, 0}));
  EXPECT_EQ(testRally.getDestination(topLeft, T::eSouthEast), (Point{0, 1}));
  EXPECT_EQ(testRally.getDestination(topLeft, T::eSouth), topLeft);
  EXPECT_EQ(testRally.getDestination(topLeft, T::eSouthWest), topLeft);
  EXPECT_EQ(testRally.getDestination(topLeft, T::eNorthWest), topLeft);

  // North and NorthWest should bounce back.
  EXPECT_EQ(testRally.getDestination(top, T::eNorth), top);
  EXPECT_EQ(testRally.getDestination(top, T::eNorthEast), (Point{3, 0}));
  EXPECT_EQ(testRally.getDestination(top, T::eSouthEast), (Point{2, 1}));
  EXPECT_EQ(testRally.getDestination(top, T::eSouth), (Point{1, 1}));
  EXPECT_EQ(testRally.getDestination(top, T::eSouthWest), (Point{1, 0}));
  EXPECT_EQ(testRally.getDestination(top, T::eNorthWest), top);

  // The North* directions should bounce back.
  EXPECT_EQ(testRally.getDestination(topRight, T::eNorth), topRight);
  EXPECT_EQ(testRally.getDestination(topRight, T::eNorthEast), topRight);
  EXPECT_EQ(testRally.getDestination(topRight, T::eSouthEast), (Point{4, 1}));
  EXPECT_EQ(testRally.getDestination(topRight, T::eSouth), (Point{3, 1}));
  EXPECT_EQ(testRally.getDestination(topRight, T::eSouthWest), (Point{3, 0}));
  EXPECT_EQ(testRally.getDestination(topRight, T::eNorthWest), topRight);

  // South and SouthWest bounce back
  EXPECT_EQ(testRally.getDestination(left, T::eNorth), (Point{1, 1}));
  EXPECT_EQ(testRally.getDestination(left, T::eNorthEast), (Point{1, 2}));
  EXPECT_EQ(testRally.getDestination(left, T::eSouthEast), (Point{0, 3}));
  EXPECT_EQ(testRally.getDestination(left, T::eSouth), left);
  EXPECT_EQ(testRally.getDestination(left, T::eSouthWest), left);
  EXPECT_EQ(testRally.getDestination(left, T::eNorthWest), (Point{0, 1}));

  // No bounceback in the middle of the map.
  EXPECT_EQ(testRally.getDestination(middle, T::eNorth), (Point{3, 1}));
  EXPECT_EQ(testRally.getDestination(middle, T::eNorthEast), (Point{3, 2}));
  EXPECT_EQ(testRally.getDestination(middle, T::eSouthEast), (Point{2, 3}));
  EXPECT_EQ(testRally.getDestination(middle, T::eSouth), (Point{1, 3}));
  EXPECT_EQ(testRally.getDestination(middle, T::eSouthWest), (Point{1, 2}));
  EXPECT_EQ(testRally.getDestination(middle, T::eNorthWest), (Point{2, 1}));

  // North and NorthEast bounce back.
  EXPECT_EQ(testRally.getDestination(right, T::eNorth), right);
  EXPECT_EQ(testRally.getDestination(right, T::eNorthEast), right);
  EXPECT_EQ(testRally.getDestination(right, T::eSouthEast), (Point{4, 3}));
  EXPECT_EQ(testRally.getDestination(right, T::eSouth), (Point{3, 3}));
  EXPECT_EQ(testRally.getDestination(right, T::eSouthWest), (Point{3, 2}));
  EXPECT_EQ(testRally.getDestination(right, T::eNorthWest), (Point{4, 1}));

  // The South* directions bounce back
  EXPECT_EQ(testRally.getDestination(bottomLeft, T::eNorth), (Point{1, 3}));
  EXPECT_EQ(testRally.getDestination(bottomLeft, T::eNorthEast), (Point{1, 4}));
  EXPECT_EQ(testRally.getDestination(bottomLeft, T::eSouthEast), bottomLeft);
  EXPECT_EQ(testRally.getDestination(bottomLeft, T::eSouth), bottomLeft);
  EXPECT_EQ(testRally.getDestination(bottomLeft, T::eSouthWest), bottomLeft);
  EXPECT_EQ(testRally.getDestination(bottomLeft, T::eNorthWest), (Point{0, 3}));

  // South and SouthEast bounce back
  EXPECT_EQ(testRally.getDestination(bottom, T::eNorth), (Point{3, 3}));
  EXPECT_EQ(testRally.getDestination(bottom, T::eNorthEast), (Point{3, 4}));
  EXPECT_EQ(testRally.getDestination(bottom, T::eSouthEast), bottom);
  EXPECT_EQ(testRally.getDestination(bottom, T::eSouth), bottom);
  EXPECT_EQ(testRally.getDestination(bottom, T::eSouthWest), (Point{1, 4}));
  EXPECT_EQ(testRally.getDestination(bottom, T::eNorthWest), (Point{2, 3}));

  // Only NorthWest and SouthWest shouldn't bounce back.
  EXPECT_EQ(testRally.getDestination(bottomRight, T::eNorth), bottomRight);
  EXPECT_EQ(testRally.getDestination(bottomRight, T::eNorthEast), bottomRight);
  EXPECT_EQ(testRally.getDestination(bottomRight, T::eSouthEast), bottomRight);
  EXPECT_EQ(testRally.getDestination(bottomRight, T::eSouth), bottomRight);
  EXPECT_EQ(testRally.getDestination(bottomRight, T::eSouthWest),
            (Point{3, 4}));
  EXPECT_EQ(testRally.getDestination(bottomRight, T::eNorthWest),
            (Point{4, 3}));
}

TEST(RallyMap, Neighbors) {
  Point topLeft{0, 0};
  Point top{2, 0};
  Point topRight{4, 0};
  Point left{0, 2};
  Point middle{2, 2};
  Point right{4, 2};
  Point bottomLeft{0, 4};
  Point bottom{2, 4};
  Point bottomRight{4, 4};

  RallyMap testRally(topLeft, bottomRight, kRandomSquareemplate);

  using namespace Direction;

  auto neighbor = testRally.getNeighbors(topLeft);
  std::sort(neighbor.begin(), neighbor.end());
  EXPECT_EQ(neighbor, (std::vector<std::pair<Point, T>>{
                          std::make_pair((Point{0, 1}), T::eSouthEast),
                          std::make_pair((Point{1, 0}), T::eNorthEast)}));

  neighbor = testRally.getNeighbors(top);
  std::sort(neighbor.begin(), neighbor.end());
  EXPECT_EQ(neighbor, (std::vector<std::pair<Point, T>>{
                          std::make_pair((Point{1, 0}), T::eSouthWest),
                          std::make_pair((Point{1, 1}), T::eSouth),
                          std::make_pair((Point{2, 1}), T::eSouthEast),
                          std::make_pair((Point{3, 0}), T::eNorthEast)}));

  neighbor = testRally.getNeighbors(topRight);
  std::sort(neighbor.begin(), neighbor.end());
  EXPECT_EQ(neighbor, (std::vector<std::pair<Point, T>>{
                          std::make_pair((Point{3, 0}), T::eSouthWest),
                          std::make_pair((Point{3, 1}), T::eSouth),
                          std::make_pair((Point{4, 1}), T::eSouthEast)}));

  neighbor = testRally.getNeighbors(left);
  std::sort(neighbor.begin(), neighbor.end());
  EXPECT_EQ(neighbor, (std::vector<std::pair<Point, T>>{
                          std::make_pair((Point{0, 1}), T::eNorthWest),
                          std::make_pair((Point{0, 3}), T::eSouthEast),
                          std::make_pair((Point{1, 1}), T::eNorth),
                          std::make_pair((Point{1, 2}), T::eNorthEast)}));

  neighbor = testRally.getNeighbors(middle);
  std::sort(neighbor.begin(), neighbor.end());
  EXPECT_EQ(neighbor, (std::vector<std::pair<Point, T>>{
                          std::make_pair((Point{1, 2}), T::eSouthWest),
                          std::make_pair((Point{1, 3}), T::eSouth),
                          std::make_pair((Point{2, 1}), T::eNorthWest),
                          std::make_pair((Point{2, 3}), T::eSouthEast),
                          std::make_pair((Point{3, 1}), T::eNorth),
                          std::make_pair((Point{3, 2}), T::eNorthEast)}));

  neighbor = testRally.getNeighbors(right);
  std::sort(neighbor.begin(), neighbor.end());
  EXPECT_EQ(neighbor, (std::vector<std::pair<Point, T>>{
                          std::make_pair((Point{3, 2}), T::eSouthWest),
                          std::make_pair((Point{3, 3}), T::eSouth),
                          std::make_pair((Point{4, 1}), T::eNorthWest),
                          std::make_pair((Point{4, 3}), T::eSouthEast)}));

  neighbor = testRally.getNeighbors(bottomLeft);
  std::sort(neighbor.begin(), neighbor.end());
  EXPECT_EQ(neighbor, (std::vector<std::pair<Point, T>>{
                          std::make_pair((Point{0, 3}), T::eNorthWest),
                          std::make_pair((Point{1, 3}), T::eNorth),
                          std::make_pair((Point{1, 4}), T::eNorthEast)}));

  neighbor = testRally.getNeighbors(bottom);
  std::sort(neighbor.begin(), neighbor.end());
  EXPECT_EQ(neighbor, (std::vector<std::pair<Point, T>>{
                          std::make_pair((Point{1, 4}), T::eSouthWest),
                          std::make_pair((Point{2, 3}), T::eNorthWest),
                          std::make_pair((Point{3, 3}), T::eNorth),
                          std::make_pair((Point{3, 4}), T::eNorthEast)}));

  neighbor = testRally.getNeighbors(bottomRight);
  std::sort(neighbor.begin(), neighbor.end());
  EXPECT_EQ(neighbor, (std::vector<std::pair<Point, T>>{
                          std::make_pair((Point{3, 4}), T::eSouthWest),
                          std::make_pair((Point{4, 3}), T::eNorthWest)}));
}

TEST(RallyMap, MoveCost) {
  RallyMap costTest(
      {3, 0}, {3, 2},
      std::vector<std::vector<uint>>{{1, 2, 3, 7}, {4, 5, 6, 8}, {7, 8, 9, 9}});

  // Simple cases
  EXPECT_EQ(costTest.getMoveCost({1, 1}, Direction::T::eNorth), 8);
  EXPECT_EQ(costTest.getMoveCost({1, 1}, Direction::T::eNorthEast), 11);
  EXPECT_EQ(costTest.getMoveCost({1, 1}, Direction::T::eSouthEast), 13);
  EXPECT_EQ(costTest.getMoveCost({1, 1}, Direction::T::eSouth), 12);
  EXPECT_EQ(costTest.getMoveCost({1, 1}, Direction::T::eSouthWest), 9);
  EXPECT_EQ(costTest.getMoveCost({1, 1}, Direction::T::eNorthWest), 7);

  // Corner case including bounceback
  EXPECT_EQ(costTest.getMoveCost({0, 2}, Direction::T::eNorth), 12);
  EXPECT_EQ(costTest.getMoveCost({0, 2}, Direction::T::eNorthEast), 15);
  EXPECT_EQ(costTest.getMoveCost({0, 2}, Direction::T::eSouthEast), 14);
  EXPECT_EQ(costTest.getMoveCost({0, 2}, Direction::T::eSouth), 14);
  EXPECT_EQ(costTest.getMoveCost({0, 2}, Direction::T::eSouthWest), 14);
  EXPECT_EQ(costTest.getMoveCost({0, 2}, Direction::T::eNorthWest), 11);

  // To and from start including bounceback
  EXPECT_EQ(costTest.getMoveCost({3, 0}, Direction::T::eNorth), 2);
  EXPECT_EQ(costTest.getMoveCost({3, 0}, Direction::T::eNorthEast), 2);
  EXPECT_EQ(costTest.getMoveCost({3, 0}, Direction::T::eSouthEast), 9);
  EXPECT_EQ(costTest.getMoveCost({3, 0}, Direction::T::eSouth), 7);
  EXPECT_EQ(costTest.getMoveCost({3, 0}, Direction::T::eSouthWest), 4);
  EXPECT_EQ(costTest.getMoveCost({3, 0}, Direction::T::eNorthWest), 2);

  EXPECT_EQ(costTest.getMoveCost({2, 0}, Direction::T::eNorthEast), 4);
  EXPECT_EQ(costTest.getMoveCost({2, 1}, Direction::T::eNorth), 7);
  EXPECT_EQ(costTest.getMoveCost({3, 1}, Direction::T::eNorthWest), 9);

  // To and from finish including bounceback
  EXPECT_EQ(costTest.getMoveCost({3, 2}, Direction::T::eNorth), 2);
  EXPECT_EQ(costTest.getMoveCost({3, 2}, Direction::T::eNorthEast), 2);
  EXPECT_EQ(costTest.getMoveCost({3, 2}, Direction::T::eSouthEast), 2);
  EXPECT_EQ(costTest.getMoveCost({3, 2}, Direction::T::eSouth), 2);
  EXPECT_EQ(costTest.getMoveCost({3, 2}, Direction::T::eSouthWest), 10);
  EXPECT_EQ(costTest.getMoveCost({3, 2}, Direction::T::eNorthWest), 9);

  EXPECT_EQ(costTest.getMoveCost({3, 1}, Direction::T::eSouthEast), 9);
  EXPECT_EQ(costTest.getMoveCost({2, 2}, Direction::T::eNorthEast), 10);
}

TEST(RallyMap, PathEnd) {
  RallyMap pathTest({0, 0}, {22, 4}, kTestTemplate);

  using namespace Direction;

  // Not moving is valid.
  EXPECT_EQ(pathTest.calculatePathEnd(std::vector<T>{}), (Point{0, 0}));

  // Single move.
  EXPECT_EQ(pathTest.calculatePathEnd(std::vector<T>{T::eNorth}),
            (Point{0, 0}));
  EXPECT_EQ(pathTest.calculatePathEnd(std::vector<T>{T::eNorthEast}),
            (Point{1, 0}));
  EXPECT_EQ(pathTest.calculatePathEnd(std::vector<T>{T::eSouthEast}),
            (Point{0, 1}));
  EXPECT_EQ(pathTest.calculatePathEnd(std::vector<T>{T::eSouth}),
            (Point{0, 0}));
  EXPECT_EQ(pathTest.calculatePathEnd(std::vector<T>{T::eSouthWest}),
            (Point{0, 0}));
  EXPECT_EQ(pathTest.calculatePathEnd(std::vector<T>{T::eNorthWest}),
            (Point{0, 0}));

  // Arbitrary movement.
  EXPECT_EQ(pathTest.calculatePathEnd(kDiagonalPath), (Point{4, 4}));
  EXPECT_EQ(pathTest.calculatePathEnd(kSpiralPath), (Point{2, 2}));
  EXPECT_EQ(pathTest.calculatePathEnd(kAllDirectionsPath), (Point{0, 0}));
  EXPECT_EQ(pathTest.calculatePathEnd(kFollowBottomToFinishPath),
            (Point{22, 4}));
  EXPECT_EQ(pathTest.calculatePathEnd(kShorterPath), (Point{22, 4}));
}

TEST(RallyMap, PathCost) {
  RallyMap pathTest({0, 0}, {22, 4}, kTestTemplate);

  // Not moving is valid.
  EXPECT_EQ(pathTest.calculatePathCost(std::vector<Direction::T>{}), 0);

  // Single move.
  EXPECT_EQ(pathTest.calculatePathCost(
                std::vector<Direction::T>{Direction::T::eNorth}),
            2);
  EXPECT_EQ(pathTest.calculatePathCost(
                std::vector<Direction::T>{Direction::T::eNorthEast}),
            9);
  EXPECT_EQ(pathTest.calculatePathCost(
                std::vector<Direction::T>{Direction::T::eSouthEast}),
            2);
  EXPECT_EQ(pathTest.calculatePathCost(
                std::vector<Direction::T>{Direction::T::eSouth}),
            2);
  EXPECT_EQ(pathTest.calculatePathCost(
                std::vector<Direction::T>{Direction::T::eSouthWest}),
            2);
  EXPECT_EQ(pathTest.calculatePathCost(
                std::vector<Direction::T>{Direction::T::eNorthWest}),
            2);

  // Arbitrary movement.
  EXPECT_EQ(pathTest.calculatePathCost(kDiagonalPath), 58);
  EXPECT_EQ(pathTest.calculatePathCost(kSpiralPath), 153);
  EXPECT_EQ(pathTest.calculatePathCost(kAllDirectionsPath), 40);
  EXPECT_EQ(pathTest.calculatePathCost(kFollowBottomToFinishPath), 220);
  EXPECT_EQ(pathTest.calculatePathCost(kShorterPath), 110);
}

TEST(RallyMap, AnalyzePath) {
  RallyMap pathTest({0, 0}, {22, 4}, kTestTemplate);
  uint cost;
  bool finished;

  // Cases that don't finish.
  std::tie(cost, finished) = pathTest.analyzePath(std::vector<Direction::T>{});
  EXPECT_EQ(cost, 0);
  EXPECT_FALSE(finished);

  std::tie(cost, finished) =
      pathTest.analyzePath(std::vector<Direction::T>{Direction::T::eNorth});
  EXPECT_EQ(cost, 2);
  EXPECT_FALSE(finished);

  std::tie(cost, finished) =
      pathTest.analyzePath(std::vector<Direction::T>{Direction::T::eNorthEast});
  EXPECT_EQ(cost, 9);
  EXPECT_FALSE(finished);

  std::tie(cost, finished) =
      pathTest.analyzePath(std::vector<Direction::T>{Direction::T::eSouthEast});
  EXPECT_EQ(cost, 2);
  EXPECT_FALSE(finished);

  std::tie(cost, finished) =
      pathTest.analyzePath(std::vector<Direction::T>{Direction::T::eSouth});
  EXPECT_EQ(cost, 2);
  EXPECT_FALSE(finished);

  std::tie(cost, finished) =
      pathTest.analyzePath(std::vector<Direction::T>{Direction::T::eSouthWest});
  EXPECT_EQ(cost, 2);
  EXPECT_FALSE(finished);

  std::tie(cost, finished) =
      pathTest.analyzePath(std::vector<Direction::T>{Direction::T::eNorthWest});
  EXPECT_EQ(cost, 2);
  EXPECT_FALSE(finished);

  std::tie(cost, finished) = pathTest.analyzePath(kDiagonalPath);
  EXPECT_EQ(cost, 58);
  EXPECT_FALSE(finished);

  std::tie(cost, finished) = pathTest.analyzePath(kSpiralPath);
  EXPECT_EQ(cost, 153);
  EXPECT_FALSE(finished);

  std::tie(cost, finished) = pathTest.analyzePath(kAllDirectionsPath);
  EXPECT_EQ(cost, 40);
  EXPECT_FALSE(finished);

  // Cases that finish.
  std::tie(cost, finished) = pathTest.analyzePath(kFollowBottomToFinishPath);
  EXPECT_EQ(cost, 220);
  EXPECT_TRUE(finished);

  std::tie(cost, finished) = pathTest.analyzePath(kShorterPath);
  EXPECT_EQ(cost, 110);
  EXPECT_TRUE(finished);
}

TEST(RallyMap, ToString) {
  RallyMap testRally({0, 4}, {22, 4}, kTestTemplate);

  EXPECT_EQ(testRally.toString(),
            ("    |\n"
             "8 8 8 8 8 1 8 8 8 8 8 1 8 8 8 8 8 1 8 8 8 8 8\n"
             " 1 1 8 1 1 1 8 1 1 1 1 1 8 1 1 1 1 1 1 1 8 1 1\n"
             "  1 1 8 1 1 1 8 8 8 8 8 1 8 8 8 8 8 1 1 1 8 1 1\n"
             "   1 1 8 1 1 1 8 1 1 1 1 1 1 1 1 1 8 1 1 1 8 1 1\n"
             "    * 1 8 1 1 1 8 8 8 8 8 1 8 8 8 8 8 1 1 1 8 1 &\n"
             "                                                |\n"));

  testRally.setEndPoints({0, 4}, {11, 2});
  EXPECT_EQ(testRally.toString(),
            ("    |\n"
             "8 8 8 8 8 1 8 8 8 8 8 1 8 8 8 8 8 1 8 8 8 8 8\n"
             " 1 1 8 1 1 1 8 1 1 1 1 1 8 1 1 1 1 1 1 1 8 1 1\n"
             "  1 1 8 1 1 1 8 8 8 8 8 & 8 8 8 8 8 1 1 1 8 1 1\n"
             "   1 1 8 1 1 1 8 1 1 1 1 1 1 1 1 1 8 1 1 1 8 1 1\n"
             "    * 1 8 1 1 1 8 8 8 8 8 1 8 8 8 8 8 1 1 1 8 1 1\n"
             "                        |\n"));

  testRally.setEndPoints({21, 1}, {11, 2});
  EXPECT_EQ(testRally.toString(),
            ("                                           |\n"
             "8 8 8 8 8 1 8 8 8 8 8 1 8 8 8 8 8 1 8 8 8 8 8\n"
             " 1 1 8 1 1 1 8 1 1 1 1 1 8 1 1 1 1 1 1 1 8 * 1\n"
             "  1 1 8 1 1 1 8 8 8 8 8 & 8 8 8 8 8 1 1 1 8 1 1\n"
             "   1 1 8 1 1 1 8 1 1 1 1 1 1 1 1 1 8 1 1 1 8 1 1\n"
             "    1 1 8 1 1 1 8 8 8 8 8 1 8 8 8 8 8 1 1 1 8 1 1\n"
             "                        |\n"));

  testRally.setEndPoints({21, 3}, {1, 1});
  EXPECT_EQ(testRally.toString(),
            ("                                             |\n"
             "8 8 8 8 8 1 8 8 8 8 8 1 8 8 8 8 8 1 8 8 8 8 8\n"
             " 1 & 8 1 1 1 8 1 1 1 1 1 8 1 1 1 1 1 1 1 8 1 1\n"
             "  1 1 8 1 1 1 8 8 8 8 8 1 8 8 8 8 8 1 1 1 8 1 1\n"
             "   1 1 8 1 1 1 8 1 1 1 1 1 1 1 1 1 8 1 1 1 8 * 1\n"
             "    1 1 8 1 1 1 8 8 8 8 8 1 8 8 8 8 8 1 1 1 8 1 1\n"
             "   |\n"));
}
