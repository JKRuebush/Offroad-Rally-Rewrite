#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>

#include "agent/agent-manager.h"
#include "map/rally-map.h"

namespace {
// So a larger number of cases are covered the size of the `RallyMap` changes
// over time. It starts at the minimum size, increments up to the max size,
// and then loops back to the smaller size. For example a min of 5x5 and a max
// of 6x6 results in the series 5x5 -> 6x5 -> 5x6 -> 6x6 -> 5x5 -> ...
const int kDefaultNumRaces = 1000;
const int kMinMapWidth = 3;
const int kMinMapHeigh = 3;
const int kMaxMapWidth = 28;
const int kMaxMapHeight = 28;
}  // namespace

using Rally::AgentManager;
using Rally::AgentWrapper;
using Rally::RallyMap;

namespace {

std::string printPath(std::vector<Direction::T> path) {
  std::string out = "";

  if(path.size() == 0) {
    return "No Path";
  }

  for(size_t i = 0; i < path.size(); ++i) {
    auto dir = path[i];

    switch(dir) {
      case Direction::T::eNorth:
        out += "N";
        break;
      case Direction::T::eNorthEast:
        out += "NE";
        break;
      case Direction::T::eSouthEast:
        out += "SE";
        break;
      case Direction::T::eSouth:
        out += "S";
        break;
      case Direction::T::eSouthWest:
        out += "SW";
        break;
      case Direction::T::eNorthWest:
        out += "NW";
        break;
      case Direction::T::eNone:
        out += "X";
        break;
    }

    if(i + 1 < path.size()) {
      out += ", ";
    }
  }

  return out;
}

}  // namespace

int main(int argc, char** argv) {
  srand(time(NULL));

  uint numRaces = kDefaultNumRaces;

  if(argc > 1) {
    try {
      int tmp = std::stoi(argv[1], nullptr, 10);

      if(tmp < 0) {
        std::cerr << "Invalid race count: " << argv[1] << std::endl;
        return EXIT_FAILURE;
      }

      numRaces = tmp;

    } catch(std::invalid_argument& e) {
      std::cerr << "Invalid race count: " << argv[1] << std::endl;
      return EXIT_FAILURE;
    }
  }

  std::vector<AgentWrapper> wrappers;
  AgentManager::GetInstance()->makeAgents(wrappers);

  uint race = 0;
  while(true) {
    for(uint y = kMinMapHeigh; y <= kMaxMapHeight; ++y) {
      for(uint x = kMinMapWidth; x <= kMaxMapWidth; ++x) {
        if(++race > numRaces) {
          goto endRaces;
        }
        RallyMap rally(x, y);

        std::cout << rally << std::endl;

        for(AgentWrapper& agent : wrappers) {
          agent.addRace(rally);
        }

        std::sort(wrappers.begin(), wrappers.end(),
                  AgentWrapper::operatorOrderLastRace);

        std::cout
            << "            Name |  Path Cost |  Map Looks | Finished | Path"
            << std::endl;

        for(const AgentWrapper& agent : wrappers) {
          std::cout << std::right << std::setw(16) << agent.getName() << " | ";
          std::cout << std::right << std::setw(10) << agent.pathCost << " | ";
          std::cout << std::right << std::setw(10) << agent.mapLooks << " | ";
          std::cout << std::right << std::setw(8)
                    << (agent.finishedRace ? "Yes" : "No") << " | ";
          std::cout << printPath(agent.path) << std::endl;
        }

        std::cout << std::endl;
      }
    }
  }
endRaces:

  std::sort(wrappers.begin(), wrappers.end(),
            AgentWrapper::operatorOrderAllRace);

  std::cout << std::endl;
  std::cout << std::string(80, '-') << "\n";
  std::cout << std::string(32, '-') << " Final Rankings "
            << std::string(32, '-') << "\n";
  std::cout << std::string(80, '-') << "\n";
  std::cout << "            Name |  Path Cost |  Map Looks | Finished"
            << std::endl;

  for(const AgentWrapper& agent : wrappers) {
    std::cout << std::right << std::setw(16) << agent.getName() << " | ";
    std::cout << std::right << std::setw(10) << agent.totalPathCost << " | ";
    std::cout << std::right << std::setw(10) << agent.totalMapLooks << " | ";
    std::cout << std::right << std::setw(8) << agent.racesFinished;
    std::cout << std::endl;
  }

  return EXIT_SUCCESS;
}
