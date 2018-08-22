#include <cstdlib>
#include <ctime>
#include <iostream>

#include "rallyAgent.h"
#include "rallyMap.h"

std::string printPath(std::vector<Direction> path) {
    std::string out = "";

    if(path.size() == 0) {
        return "No Path";
    }

    for(size_t i = 0; i < path.size(); ++i) {
        auto dir = path[i];

        switch(dir) {
            case Direction::North:
                out += "N";
                break;
            case Direction::NorthEast:
                out += "NE";
                break;
            case Direction::SouthEast:
                out += "SE";
                break;
            case Direction::South:
                out += "S";
                break;
            case Direction::SouthWest:
                out += "SW";
                break;
            case Direction::NorthWest:
                out += "NW";
                break;
        }

        if(i + 1 < path.size()) {
            out += ", ";
        }
    }

    return out;
}

int main() {
    srand(time(NULL));

    auto agents = AgentManager::GetInstance()->makeAgents();

    std::cout << agents.size() << std::endl;

    RallyMap test(9, 9);

    std::cout << test << std::endl;

    Point endPoint;
    uint pathCost;
    bool finishedRace;

    for(auto agent : agents) {
        MapInterface api(&test);
        auto path = agent->RunAgent(&api);
        std::cout << "Name: " << agent->getName() << std::endl;
        std::cout << "Path: " << printPath(path) << std::endl;
        std::cout << "Map Looks: " << api.getMapLooks() << std::endl;
        std::tie(endPoint, pathCost, finishedRace) = test.analyzePath(path);
        std::cout << "Path Cost: " << pathCost << std::endl;
        std::cout << "Finished Race: " << (finishedRace ? "Yes" : "No") << std::endl;
        std::cout << std::endl;
        delete agent;
    }

    return EXIT_SUCCESS;
}
