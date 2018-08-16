#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <random>

#include "rallyMap.h"

int main() {

    srand(time(NULL));

    RallyMap test(Point(4,2), Point(0,0), std::vector<std::vector<uint>> {
        {8,8,8,8,8,7,8,8,8,8,8,7,8,8,8,8,8,7,8,8,8,8,8},
        {7,7,8,7,7,7,8,7,7,7,7,7,8,7,7,7,7,7,7,7,8,7,7},
        {7,7,8,7,7,7,8,8,8,8,8,7,8,8,8,8,8,7,7,7,8,7,7},
        {7,7,8,7,7,7,8,7,7,7,7,7,7,7,7,7,8,7,7,7,8,7,7},
        {7,7,8,7,7,7,8,8,8,8,8,7,8,8,8,8,8,7,7,7,8,7,7}
    });
    std::cout << test << std::endl;

    test.setFinish(test.getDestination(test.getStart(), Direction::North));
    std::cout << test << std::endl;

    test.setFinish(test.getDestination(test.getStart(), Direction::NorthEast));
    std::cout << test << std::endl;

    test.setFinish(test.getDestination(test.getStart(), Direction::SouthEast));
    std::cout << test << std::endl;

    test.setFinish(test.getDestination(test.getStart(), Direction::South));
    std::cout << test << std::endl;

    test.setFinish(test.getDestination(test.getStart(), Direction::SouthWest));
    std::cout << test << std::endl;

    test.setFinish(test.getDestination(test.getStart(), Direction::NorthWest));
    std::cout << test << std::endl;

    return EXIT_SUCCESS;
}
