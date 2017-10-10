#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <deque>
#include <tuple>
#include <random>
#include <iterator>
#include <functional>
#include <algorithm>
#include "../ShipSpec.hpp"
#include "../Simulation.hpp"

using namespace Simulation;

ShipSpec readShip()
{
  std::cout << "Enter ship details:\n";
  std::cout << "hull shield computer yellow orange red initiative:\n";
  ShipSpec ship;
  std::cin >> ship.hull >> ship.shield >> ship.computer >> ship.yellowGuns >> ship.orangeGuns
    >> ship.redGuns >> ship.initiative;

  return ship;
}


std::ostream& log(bool toFile = true)
{
  if (toFile)
  {
    static std::ofstream file("logFile");
    return file;
  }
  return std::cout;
}


void readFleets()
{
  std::cout << "Eclipse Simulator!\nShip A:" << std::endl;

  auto shipA = readShip();

  std::cout << "Ship B:" << std::endl;
  auto shipB = readShip();

  std::cout << "Read these ships:\n"
    << shipA << "\n" << shipB << std::endl;
}

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cout << "Usage: eclipse NUM_INTERCEPTORS [NUM_ANCIENTS=1] [NUM_TRIALS=1000]" << std::endl;
    return 1;
  }

  auto playerInts = argv[1];
  auto ancients = argc > 2 ? argv[2] : "1";
  std::cout << playerInts << " base interceptor(s) vs " << ancients << " ancient interceptor(s)" << std::endl;

  ShipSpec playerInter { 0, 0, 0, 1, 0, 0, 3 };
  ShipSpec ancientInter { 2, 0, 1, 2, 0, 0, 2 };

  AttackingFleet player { { "player" }, {} };
  player.addNewShip(playerInter, atoi(playerInts));
  DefendingFleet ancient { { "ancients" }, {}};
  ancient.addNewShip(ancientInter, atoi(ancients));

  auto trials = argc > 3 ? atoi(argv[3]) : 1000;
  simulateBattle(player, ancient, trials);

  return 0;
}
