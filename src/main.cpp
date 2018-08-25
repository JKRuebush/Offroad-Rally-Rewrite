#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>

#include "rallyAgent.h"
#include "rallyMap.h"

namespace {
// So a larger number of cases are covered the size of the `RallyMap` changes
// over time. It starts at the minimum size, increments up to the max size,
// and then loops back to the smaller size. For example a min of 5x5 and a max
// of 6x6 results in the series 5x5 -> 6x5 -> 5x6 -> 6x6 -> 5x5 -> ...
const int DEFAULT_NUM_RACES = 1000;
const int MIN_MAP_WIDTH = 3;
const int MIN_MAP_HEIGHT = 3;
const int MAX_MAP_WIDTH = 12;
const int MAX_MAP_HEIGHT = 12;
}  // namespace

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
            case Direction::None:
                out += "X";
                break;
        }

        if(i + 1 < path.size()) {
            out += ", ";
        }
    }

    return out;
}

int main(int argc, char** argv) {
    srand(time(NULL));

    uint numRaces = DEFAULT_NUM_RACES;

    if(argc > 1) {
        try {
            int tmp = std::stoi(argv[1], nullptr, 10);

            if(tmp < 0) {
                std::cerr << "Invalid race count: " << argv[1] << std::endl;
                return EXIT_FAILURE;
            }

            numRaces = tmp;

        } catch(std::invalid_argument e) {
            std::cerr << "Invalid race count: " << argv[1] << std::endl;
            return EXIT_FAILURE;
        }
    }

    std::vector<AgentWrapper> agents;
    AgentManager::GetInstance()->makeAgents(agents);

    uint race = 0;
    while(true) {
        for(uint y = MIN_MAP_HEIGHT; y <= MAX_MAP_HEIGHT; ++y) {
            for(uint x = MIN_MAP_WIDTH; x <= MAX_MAP_WIDTH; ++x) {
                if(++race > numRaces) {
                    goto endRaces;
                }
                RallyMap rally(x, y);

                std::cout << rally << std::endl;

                for(size_t i = 0; i < agents.size(); ++i) {
                    AgentWrapper& wrapper = agents[i];
                    wrapper.addRace(&rally);
                }

                std::sort(agents.begin(), agents.end(), AgentWrapper::orderLastRace);

                std::cout << "            Name |  Path Cost |  Map Looks | Finished | Path"
                          << std::endl;

                for(size_t i = 0; i < agents.size(); ++i) {
                    AgentWrapper& wrapper = agents[i];
                    std::cout << std::right << std::setw(16) << wrapper.getName() << " | ";
                    std::cout << std::right << std::setw(10) << wrapper.pathCost << " | ";
                    std::cout << std::right << std::setw(10) << wrapper.mapLooks << " | ";
                    std::cout << std::right << std::setw(8) << (wrapper.finishedRace ? "Yes" : "No")
                              << " | ";
                    std::cout << printPath(wrapper.path) << std::endl;
                }

                std::cout << std::endl;
            }
        }
    }
endRaces:

    std::sort(agents.begin(), agents.end(), AgentWrapper::orderAllRace);

    std::cout << std::endl;
    std::cout << std::string(80, '-') << "\n";
    std::cout << std::string(32, '-') << " Final Rankings " << std::string(32, '-') << "\n";
    std::cout << std::string(80, '-') << "\n";
    std::cout << "            Name |  Path Cost |  Map Looks | Finished" << std::endl;

    for(size_t i = 0; i < agents.size(); ++i) {
        AgentWrapper& wrapper = agents[i];
        std::cout << std::right << std::setw(16) << wrapper.getName() << " | ";
        std::cout << std::right << std::setw(10) << wrapper.totalPathCost << " | ";
        std::cout << std::right << std::setw(10) << wrapper.totalMapLooks << " | ";
        std::cout << std::right << std::setw(8) << wrapper.racesFinished;
        std::cout << std::endl;
    }

    return EXIT_SUCCESS;
}
