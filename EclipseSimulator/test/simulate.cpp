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

std::ostream& log(bool toFile = true)
{
  if (toFile)
  {
    static std::ofstream file("logFile");
    return file;
  }
  return std::cout;
}

class DamageApplier
{
public:
  DamageApplier(const FightingShip& attacker) : attacker_(attacker)
  {
    roll_ = attack(attacker.spec());
    log() << "  Roll: \t" << roll_;
  }
  void operator()(FightingShip& target)
  {
    if (attacker_.isFighting(target))
    {
      auto result = resultOfAttack(attacker_.spec(), roll_, target.spec());

      for (auto i = 0; i < result.yellowDice.size(); ++i)
      {
        if (result.yellowDice[i] && target.isAlive())
        {
          log() << "\t hits.\n";
          target.applyDamage(1);
          log() << "Target is now " << target << std::endl;
          roll_.yellowDice[i] = 1;
        }
      }
    }
  }
private:
  const FightingShip& attacker_;
  AttackRoll roll_;
};

class Battle
{
public:
  Battle(const AttackingFleet& attacker, const DefendingFleet& defender)
  {
    std::copy(defender.ships().begin(), defender.ships().end(), std::back_inserter(allShips_));
    std::copy(attacker.ships().begin(), attacker.ships().end(), std::back_inserter(allShips_));
    std::sort(allShips_.begin(), allShips_.end());

    // std::cout << "Battle with these sorted ships:\n";
    // auto i = 1;
    // for (const auto& ship : allShips_)
    // {
    //   std::cout << i++ << "\t" << ship << "\n";
    // }
    // std::cout << std::endl;
  }

  bool oneRound()
  {
    if (battleComplete())
      return false;

    auto i = 1;
    while (firedShips_.size() < allShips_.size())
    {
      auto attacker = allShips_.front();
      allShips_.pop_front();

      log() << i++;
      DamageApplier applyDamage(attacker);
      std::for_each(allShips_.begin(), allShips_.end(), applyDamage);

      auto deadShipCleanup = [](const FightingShip& ship) { return !ship.isAlive(); };
      //std::cout << "\t\t~~~allShips before cleanup: " << allShips_.size();
      allShips_.erase(std::remove_if(allShips_.begin(), allShips_.end(), deadShipCleanup), allShips_.end());
      //std::cout << " after cleanup: " << allShips_.size() << std::endl;
      firedShips_.push_back(attacker);
      allShips_.push_back(attacker);

      if (battleComplete())
      {
        log() << "\nBATTLE COMPLETE: victor is " << victorString_ << std::endl;
        return false;
      }
    }
    firedShips_.clear();

    return true;
  }

  std::string fightToDeath()
  {
    do
    {
      roundCount_++;
      log() << "Round " << roundCount_ << ":\n";
    }
    while (oneRound());
    return victorString_;
  }

private:
  std::deque<FightingShip> allShips_, firedShips_;
  int roundCount_{0};
  std::string victorString_;

  bool battleComplete()
  {
    if (std::all_of(allShips_.begin(), allShips_.end(), [](const FightingShip& ship) { return ship.isAttacker(); }))
    {
      victorString_ = "Attacker";
      return true;
    }
    if (std::all_of(allShips_.begin(), allShips_.end(), [](const FightingShip& ship) { return !ship.isAttacker(); }))
    {
      victorString_ = "Defender";
      return true;
    }
    return false;
  }
};

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
  std::map<std::string, int> results;
  for (int i = 0; i < trials; ++i)
  {
    Battle battle(player, ancient);
    auto result = battle.fightToDeath();
    results[result]++;
    log() << result << " wins" << std::endl;
  }

  std::cout << "After " << trials << " simulations, the results are: \n";
  for (const auto& result : results)
  {
    std::cout << result.first << " won " << result.second << " times.\n";
  }

  return 0;
}
