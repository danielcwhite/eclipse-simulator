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

}
#endif
