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

  Fleet(const std::string& name, std::initializer_list<Ship> ships) : name_(name)
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
  const std::string& name() const { return name_; }

private:
  ShipList ships_;
  std::string name_;
};

std::ostream& operator<<(std::ostream& o, const Fleet& fleet)
{
  o << fleet.name() << "\n";
  for (const auto& s : fleet.ships())
    o << s << "\n";
  return o;
}

void oneRoundOfCombat(Fleet& attacker, Fleet& defender)
{
  if (&attacker == &defender)
  {
    std::cout << "Cannot attack yourself!" << std::endl;
    return;
  }

  std::cout << "Round of combat:\nAttacker\n" << attacker
    << "Defender\n" << defender << std::endl;
}

int main(int argc, char** argv)
{
  std::cout << "Eclipse Simulator!\nShip A:" << std::endl;

  auto shipA = readShip();

  std::cout << "Ship B:" << std::endl;
  auto shipB = readShip();

  std::cout << "Read these ships:\n"
    << shipA << "\n" << shipB << std::endl;

  Fleet f { {"red"}, { shipA, shipB }};

  std::cout << "Here is the fleet:\n";
  for (const auto& s : f.ships())
    std::cout << s << "\n";

  Fleet f2(f);
  oneRoundOfCombat(f, f2);

  return 0;
}
