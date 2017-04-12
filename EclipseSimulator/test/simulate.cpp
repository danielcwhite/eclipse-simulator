#include <iostream>
#include <vector>
#include <tuple>
#include <random>
#include <iterator>

int roll()
{
  static std::random_device rd;  //Will be used to obtain a seed for the random number engine
  static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
  static std::uniform_int_distribution<> dis(1, 6);
  return dis(gen);
}

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

using GunRoll = std::vector<int>;

std::ostream& operator<<(std::ostream& o, const GunRoll& g)
{
  std::copy(g.cbegin(), g.cend(), std::ostream_iterator<int>(o, ", "));
  return o;
}

struct AttackRoll
{
  GunRoll yellowDice;
  GunRoll orangeDice;
  GunRoll redDice;
};

std::ostream& operator<<(std::ostream& o, const AttackRoll& g)
{
  if (!g.yellowDice.empty())
    o << "Yellow: " << g.yellowDice << "\n";
  if (!g.orangeDice.empty())
    o << "Orange: " << g.orangeDice << "\n";
  if (!g.redDice.empty())
    o << "Red: " << g.redDice << "\n";
  return o;
}

GunRoll rollGuns(const Ship& ship, int Ship::*gun)
{
  GunRoll gunRoll;
  for (int i = 0; i < ship.*gun; ++i)
    gunRoll.push_back(roll() + ship.computer);
  return gunRoll;
}

AttackRoll attack(const Ship& ship)
{
  return {
    rollGuns(ship, &Ship::yellowGuns),
    rollGuns(ship, &Ship::orangeGuns),
    rollGuns(ship, &Ship::redGuns)
  };
}

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

const Ship& ship(const ShipWithHitPoints& shp)
{
  return std::get<0>(shp);
}

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

  std::cout << "Round of combat:\nAttacker\n" << attacker << "\n";

  for (const auto& s : attacker.ships())
    std::cout << attack(ship(s)) << std::endl;

  std::cout  << "\nDefender\n" << defender << std::endl;

  for (const auto& s : defender.ships())
    std::cout << attack(ship(s)) << std::endl;
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
  Ship shipA {1,0,0,1,0,0,2};
  Ship shipB {2,0,1,2,0,0,1};

  Fleet f1 { {"red"}, { shipA }};
  Fleet f2 { {"blue"}, { shipB }};

  oneRoundOfCombat(f1, f2);

  return 0;
}
