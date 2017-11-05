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
#include <sstream>
#include <memory>
#include <ShipSpec.hpp>
#include <ShipInterface.hpp>

namespace Simulation
{

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

inline bool operator<(const Ship& a, const Ship& b)
{
  return AttackOrder()(a, b);
}

struct FightingShip
{
  FightingShip(const ShipSpec& ship, bool isAttacker) :
    ship_(ship), isAttacker_(isAttacker) {}
  const ShipSpec& spec() const { return ship_.spec; }
  const Ship& ship() const { return ship_; }
  bool isFighting(const FightingShip& other) const
  {
    return isAttacker_ != other.isAttacker_;
  }
  void applyDamage(int damage)
  {
    ship_.hitPoints -= damage;
  }
  bool isAlive() const
  {
    return ship_.hitPoints > 0;
  }
  bool isAttacker() const { return isAttacker_; }
private:
  Ship ship_;
  bool isAttacker_;
};

inline bool operator<(const FightingShip& lhs, const FightingShip& rhs)
{
  return std::make_tuple(lhs.ship(), lhs.isAttacker()) <
    std::make_tuple(rhs.ship(), rhs.isAttacker());
}

// Defender < Attacker ==> Def = 0/false, Atk = 1/true
struct Attacker : FightingShip
{
  explicit Attacker(const ShipSpec& spec) : FightingShip(spec, true) {}
};

struct Defender : FightingShip
{
  explicit Defender(const ShipSpec& spec) : FightingShip(spec, false) {}
};

inline std::ostream& operator<<(std::ostream& o, const Ship& ship)
{
  return o << ship.spec << " : " << ship.hitPoints << "/"
    << (ship.spec.hull+1) << " hp";
}

inline std::ostream& operator<<(std::ostream& o, const FightingShip& fs)
{
  return o << (fs.isAttacker() ? "ATK: " : "DEF: ") << fs.ship();
}



template <class ShipType>
class FleetSpec
{
public:

  using NamedFightingShip = std::tuple<ShipType, std::string>;
  using ShipList = std::vector<NamedFightingShip>;

  FleetSpec(const std::string& name, std::initializer_list<ShipSpec> ships) : name_(name)
  {
    for (const auto& ship : ships)
      addNewShip("<unknown>", ship, 1);
  }

  void addNewShip(const std::string& name, const ShipSpec& ship, int count)
  {
    for (int i = 0; i < count; ++i)
      ships_.emplace_back(ship, name);
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

using Logger = std::function<void(const std::string&)>;

class HasLogger
{
public:
  explicit HasLogger(Logger log = [](const std::string&) {}) : log_(log) {}
protected:
  template <typename T, typename... Ts>
  void log(T&& first, Ts&&... rest) const
  {
    print(std::forward<T>(first));
    log(std::forward<Ts>(rest)...);
  }
  void log() const
  {
    print("\n");
  }
  Logger logger() const { return log_; }
private:
  Logger log_;

  template <typename T>
  void print(T&& t) const
  {
    if (log_)
    {
      std::ostringstream ostr;
      ostr << t;
      log_(ostr.str());
    }
  }
};

class BattleHelper : public HasLogger
{
public:
  BattleHelper(Logger log);
  void simulateBattle(AttackingFleet& attacker, DefendingFleet& defender, int trials);
};

class DamageApplier : public HasLogger
{
public:
  DamageApplier(const ShipPtr& attacker, Logger log);
  void operator()(ShipPtr target);

private:
  int roll();
  OneGunRoll rollGuns(const ShipSpec& ship, int ShipSpec::*gun);
  AttackRoll attack(const ShipSpec& ship);
  std::function<HitResult(const OneGunRoll&)> resultOfAttackPart(int computer, int shield);
  ResultOfRoll resultOfAttack(const ShipSpec& shooter, const AttackRoll& roll, const ShipSpec& target);

  ShipPtr attacker_;
  AttackRoll roll_;
};

/*
class Battle : public HasLogger
{
public:
  Battle(const AttackingFleet& attacker, const DefendingFleet& defender, Logger log);
  bool oneRound();
  std::string fightToDeath();
private:
  std::deque<FightingShip> allShips_, firedShips_;
  int roundCount_{0};
  std::string victorString_;

  bool battleComplete();
};
*/

}

class ShipFactory
{
public:
  virtual ~ShipFactory() {}
  virtual ShipPtr make(const std::tuple<Simulation::FightingShip, std::string>& namedShip) const = 0;
};

using ShipFactoryPtr = std::shared_ptr<ShipFactory>;

#endif
