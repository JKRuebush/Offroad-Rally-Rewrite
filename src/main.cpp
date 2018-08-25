#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>

#include "rallyAgent.h"
#include "rallyMap.h"

namespace {
// Races are done in groups. Each group of races takes places over maps of
// increasing size. So with a min size of 3x3 and max of 5x5 nine races would
// take place with map sizes of 3x3, 3x4, 3x5, 4x3, 4x4, 4x5, 5x3, 5x4, 5x5.
const int NUM_RACE_GROUPS = 300;
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

int main() {
    srand(time(NULL));

    std::vector<AgentWrapper> agents;
    AgentManager::GetInstance()->makeAgents(agents);

    for(uint group = 0; group < NUM_RACE_GROUPS; ++group) {
        for(uint y = MIN_MAP_HEIGHT; y <= MAX_MAP_HEIGHT; ++y) {
            for(uint x = MIN_MAP_WIDTH; x <= MAX_MAP_WIDTH; ++x) {
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
