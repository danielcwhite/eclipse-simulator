#include <iostream>
#include <vector>
#include <tuple>
#include <random>
#include <iterator>
#include <functional>

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

template <typename T>
using Roll = std::vector<T>;

using OneGunRoll = Roll<int>;
using HitResult = Roll<bool>;

template <typename T>
std::ostream& operator<<(std::ostream& o, const Roll<T>& g)
{
  std::copy(g.cbegin(), g.cend(), std::ostream_iterator<T>(o, ","));
  return o;
}

template <typename T>
struct GunRoll
{
  Roll<T> yellowDice;
  Roll<T> orangeDice;
  Roll<T> redDice;
};

using AttackRoll = GunRoll<int>;
using ResultOfRoll = GunRoll<bool>;

template <typename T>
std::ostream& operator<<(std::ostream& o, const GunRoll<T>& g)
{
  if (!g.yellowDice.empty())
    o << "Yellow: " << g.yellowDice << "\n";
  if (!g.orangeDice.empty())
    o << "Orange: " << g.orangeDice << "\n";
  if (!g.redDice.empty())
    o << "Red: " << g.redDice << "\n";
  return o;
}

OneGunRoll rollGuns(const Ship& ship, int Ship::*gun)
{
  OneGunRoll gunRoll;
  for (int i = 0; i < ship.*gun; ++i)
    gunRoll.push_back(roll());
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

std::function<HitResult(const OneGunRoll&)> resultOfAttackPart(int computer, int shield)
{
  return [=](const OneGunRoll& roll)
  {
    HitResult result;
    for (auto die : roll)
    {
      if (1 == die)
        result.push_back(false);
      else if (6 == die)
        result.push_back(true);
      else
        result.push_back(die + computer - shield >= 6);
    }
    return result;
  };
}

ResultOfRoll resultOfAttack(const Ship& shooter, const AttackRoll& roll, const Ship& target)
{
  auto attackFunc = resultOfAttackPart(shooter.computer, target.shield);
  return {
    attackFunc(roll.yellowDice),
    attackFunc(roll.orangeDice),
    attackFunc(roll.redDice)
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

std::ostream& operator<<(std::ostream& o, const ShipWithHitPoints& s)
{
  const auto& shipDesc = ship(s);
  return o << shipDesc << " : " << std::get<1>(s) << "/" << (shipDesc.hull+1) << " hp";
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

void printTestAttackResult(Fleet& attacker, Fleet& defender)
{
  for (const auto& s : attacker.ships())
  {
    auto roll = attack(ship(s));
    std::cout << "Roll: " << roll;
    std::cout << "Compare to targets:\n";
    for (const auto& d : defender.ships())
    {
      std::cout << "\t" << resultOfAttack(ship(s), roll, ship(d));
    }
  }
}

void oneRoundOfCombat(Fleet& attacker, Fleet& defender)
{
  if (&attacker == &defender)
  {
    std::cout << "Cannot attack yourself!" << std::endl;
    return;
  }

  std::cout << "Round of combat:\nAttacker: " << attacker << "\n";
  printTestAttackResult(attacker, defender);
  std::cout  << "\nDefender: " << defender << std::endl;
  printTestAttackResult(defender, attacker);
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

int main1(int argc, char** argv)
{
  Ship shipA {1,0,0,1,0,0,2};
  Ship shipB {2,0,1,2,0,0,1};

  Fleet f1 { {"red"}, { shipA }};
  Fleet f2 { {"blue"}, { shipB }};

  oneRoundOfCombat(f1, f2);

  return 0;
}

int main(int argc, char** argv)
{
  if (argc < 2)
    return 1;

  std::cout << argv[1] << " base interceptors vs 1 ancient interceptor" << std::endl;

  Ship playerInter { 0, 0, 0, 1, 0, 0, 2 };
  Ship ancientInter { 2, 0, 1, 2, 0, 0, 1 };

  Fleet player { { "player" }, {} };
  player.addNewShip(playerInter, atoi(argv[1]));
  Fleet ancient { { "ancients" }, { ancientInter }};

  oneRoundOfCombat(player, ancient);

  return 0;
}
