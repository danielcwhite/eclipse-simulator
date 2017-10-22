#ifndef SIMULATION_CODE
#define SIMULATION_CODE

#include <vector>
#include <map>
#include <deque>
#include <tuple>
#include <random>
#include <iterator>
#include <functional>
#include <algorithm>
#include "ShipSpec.hpp"

namespace Simulation
{

int roll()
{
  static std::random_device rd;  //Will be used to obtain a seed for the random number engine
  static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
  static std::uniform_int_distribution<> dis(1, 6);
  return dis(gen);
}

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
struct Ship
{
  explicit Ship(const ShipSpec& s) :
    spec(s), hitPoints(s.hull + 1)
  {}
  Ship(const ShipSpec& s, int hp) :
    spec(s), hitPoints(hp)
  {}
  ShipSpec spec;
  int hitPoints;
};

struct AttackOrder
{
  bool operator()(const Ship& a, const Ship& b) const
  {
    return a.spec.initiative > b.spec.initiative;
  }
};

bool operator<(const Ship& a, const Ship& b)
{
  return AttackOrder()(a, b);
}

struct FightingShip : std::tuple<Ship, bool>
{
  FightingShip(const ShipSpec& ship, bool isAttacker) :
    std::tuple<Ship, bool>(ship, isAttacker) {}
  const ShipSpec& spec() const { return std::get<0>(*this).spec; }
  bool isFighting(const FightingShip& other) const
  {
    return std::get<1>(*this) != std::get<1>(other);
  }
  void applyDamage(int damage)
  {
    std::get<0>(*this).hitPoints -= damage;
  }
  bool isAlive() const
  {
    return std::get<0>(*this).hitPoints > 0;
  }
  bool isAttacker() const { return std::get<1>(*this); }
};

// Defender < Attacker ==> Def = 0/false, Atk = 1/true
struct Attacker : FightingShip
{
  explicit Attacker(const ShipSpec& ship) : FightingShip(ship, true) {}
};

struct Defender : FightingShip
{
  explicit Defender(const ShipSpec& ship) : FightingShip(ship, false) {}
};

std::ostream& operator<<(std::ostream& o, const Ship& ship)
{
  return o << ship.spec << " : " << ship.hitPoints << "/"
    << (ship.spec.hull+1) << " hp";
}

std::ostream& operator<<(std::ostream& o, const FightingShip& fs)
{
  return o << std::get<0>(fs) << " " << (std::get<1>(fs) ? "atk " : "def ") << &fs;
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


class DamageApplier
{
public:
  DamageApplier(const FightingShip& attacker);
  void operator()(FightingShip& target);
private:
  const FightingShip& attacker_;
  AttackRoll roll_;
};

class Battle
{
public:
  Battle(const AttackingFleet& attacker, const DefendingFleet& defender);
  bool oneRound();
  std::string fightToDeath();
private:
  std::deque<FightingShip> allShips_, firedShips_;
  int roundCount_{0};
  std::string victorString_;

  bool battleComplete();
};

void simulateBattle(AttackingFleet& attacker, DefendingFleet& defender, int trials);

}
#endif
