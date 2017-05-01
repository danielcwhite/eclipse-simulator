#include <iostream>
#include <vector>
#include <deque>
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

struct ShipSpec
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

OneGunRoll rollGuns(const ShipSpec& ship, int ShipSpec::*gun)
{
  OneGunRoll gunRoll;
  for (int i = 0; i < ship.*gun; ++i)
    gunRoll.push_back(roll());
  return gunRoll;
}

AttackRoll attack(const ShipSpec& ship)
{
  return {
    rollGuns(ship, &ShipSpec::yellowGuns),
    rollGuns(ship, &ShipSpec::orangeGuns),
    rollGuns(ship, &ShipSpec::redGuns)
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

ResultOfRoll resultOfAttack(const ShipSpec& shooter, const AttackRoll& roll, const ShipSpec& target)
{
  auto attackFunc = resultOfAttackPart(shooter.computer, target.shield);
  return {
    attackFunc(roll.yellowDice),
    attackFunc(roll.orangeDice),
    attackFunc(roll.redDice)
  };
}

ShipSpec readShip()
{
  std::cout << "Enter ship details:\n";
  std::cout << "hull shield computer yellow orange red initiative:\n";
  ShipSpec ship;
  std::cin >> ship.hull >> ship.shield >> ship.computer >> ship.yellowGuns >> ship.orangeGuns
    >> ship.redGuns >> ship.initiative;

  return ship;
}

std::ostream& operator<<(std::ostream& o, const ShipSpec& ship)
{
  return o << "ShipSpec(h" << ship.hull
    << ",s" << ship.shield
    << ",c" << ship.computer
    << "," << ship.yellowGuns
    << "d1," << ship.orangeGuns
    << "d2," << ship.redGuns
    << "d4,i" << ship.initiative << ")";
}

struct Ship
{
  explicit Ship(const ShipSpec& s) :
    spec(s), hitPoints(s.hull + 1)
  {}
  Ship(const ShipSpec& s, int hp) :
    spec(s), hitPoints(hp)
  {}
  const ShipSpec spec;
  int hitPoints;

  bool operator<(const Ship& other) const
  {
    return spec.initiative < other.spec.initiative;
  }

  bool operator>(const Ship& other) const
  {
    return spec.initiative > other.spec.initiative;
  }
};

struct FightingShip : std::tuple<Ship, bool>
{
  FightingShip(const ShipSpec& ship, bool isDefender) :
    std::tuple<Ship, bool>(ship, isDefender) {}
  const ShipSpec& spec() const { return std::get<0>(*this).spec; }
};

// Defender > Attacker ==>
struct Attacker : FightingShip
{
  explicit Attacker(const ShipSpec& ship) : FightingShip(ship, false) {}
};

struct Defender : FightingShip
{
  explicit Defender(const ShipSpec& ship) : FightingShip(ship, true) {}
};

std::ostream& operator<<(std::ostream& o, const Ship& ship)
{
  return o << ship.spec << " : " << ship.hitPoints << "/"
    << (ship.spec.hull+1) << " hp";
}

template <class ShipType>
class FleetSpec
{
public:

  using ShipList = std::vector<ShipType>;

  FleetSpec(const std::string& name, std::initializer_list<ShipSpec> ships) : name_(name)
  {
    for (const auto& ship : ships)
      addNewShip(ship, 1);
  }

  void addNewShip(const ShipSpec& ship, int count)
  {
    for (int i = 0; i < count; ++i)
      ships_.emplace_back(ship);
  }

  const ShipList& ships() const { return ships_; }
  const std::string& name() const { return name_; }

private:
  ShipList ships_;
  std::string name_;
};

using AttackingFleet = FleetSpec<Attacker>;
using DefendingFleet = FleetSpec<Defender>;

template <class ShipType>
std::ostream& operator<<(std::ostream& o, const FleetSpec<ShipType>& fleet)
{
  o << fleet.name() << "\n";
  for (const auto& s : fleet.ships())
    o << s.spec() << "\n";
  return o;
}

void printTestAttackResult(const AttackingFleet& attacker, const DefendingFleet& defender)
{
  for (const auto& a : attacker.ships())
  {
    auto roll = attack(a.spec());
    std::cout << "Roll: " << roll;
    std::cout << "Compare to targets:\n";
    for (const auto& d : defender.ships())
    {
      std::cout << "\t" << resultOfAttack(a.spec(), roll, d.spec());
    }
  }
}

class Battle
{
public:
  Battle(const AttackingFleet& attacker, const DefendingFleet& defender)
  {

  }
private:
  std::deque<FightingShip> allShips_, firedShips_;
};

void oneRoundOfCombat(const AttackingFleet& attacker, const DefendingFleet& defender)
{
  std::cout << "Round of combat:\nAttacker: " << attacker << "\n";
  printTestAttackResult(attacker, defender);
  //std::cout  << "\nDefender: " << defender << std::endl;
  //printTestAttackResult(defender, attacker);
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
    return 1;

  std::cout << argv[1] << " base interceptors vs 1 ancient interceptor" << std::endl;

  ShipSpec playerInter { 0, 0, 0, 1, 0, 0, 2 };
  ShipSpec ancientInter { 2, 0, 1, 2, 0, 0, 1 };

  AttackingFleet player { { "player" }, {} };
  player.addNewShip(playerInter, atoi(argv[1]));
  DefendingFleet ancient { { "ancients" }, { ancientInter }};

  Battle battle(player, ancient);
  //battle.oneRound();
  oneRoundOfCombat(player, ancient);

  return 0;
}
