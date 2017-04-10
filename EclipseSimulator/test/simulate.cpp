#include <iostream>
#include <vector>
#include <tuple>

struct Ship
{
  int hull;
  int shield;
  int computer;
  int yellowGuns;
  int orangeGuns;
  int redGuns;
  int initiative;
};

Ship readShip()
{
  std::cout << "Enter ship details:\n";
  std::cout << "hull shield computer yellow orange red initiative:\n";
  Ship ship;
  std::cin >> ship.hull >> ship.shield >> ship.computer >> ship.yellowGuns >> ship.orangeGuns
    >> ship.redGuns >> ship.initiative;

  return ship;
}

std::ostream& operator<<(std::ostream& o, const Ship& ship)
{
  return o << "Ship(h" << ship.hull
    << ",s" << ship.shield
    << ",c" << ship.computer
    << "," << ship.yellowGuns
    << "d1," << ship.orangeGuns
    << "d2," << ship.redGuns
    << "d4,i" << ship.initiative << ")";
}

using ShipWithHitPoints = std::tuple<Ship, int>;

std::ostream& operator<<(std::ostream& o, const ShipWithHitPoints& ship)
{
  const auto& shipDesc = std::get<0>(ship);
  return o << shipDesc << " : " << std::get<1>(ship) << "/" << (shipDesc.hull+1) << " hp";
}

class Fleet
{
public:

  using ShipList = std::vector<ShipWithHitPoints>;

  Fleet(std::initializer_list<Ship> ships)
  {
    for (const auto& ship : ships)
      addNewShip(ship, 1);
  }

  void addNewShip(const Ship& ship, int count)
  {
    for (int i = 0; i < count; ++i)
      ships_.emplace_back(ship, ship.hull + 1);
  }

  const ShipList& ships() const { return ships_; }

private:
  ShipList ships_;
};

int main(int argc, char** argv)
{
  std::cout << "Eclipse Simulator!\nShip A:" << std::endl;

  auto shipA = readShip();

  std::cout << "Ship B:" << std::endl;
  auto shipB = readShip();

  std::cout << "Read these ships:\n"
    << shipA << "\n" << shipB << std::endl;

  Fleet f { shipA, shipB };

  std::cout << "Here is the fleet:\n";
  for (const auto& s : f.ships())
    std::cout << s << "\n";


  return 0;
}
