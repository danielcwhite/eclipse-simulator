#ifndef DAMAGE_APPLIER
#define DAMAGE_APPLIER

#include <Simulation.hpp>

namespace Simulation
{

class DamageApplier : public HasLogger
{
public:
  DamageApplier(const ShipPtr& attacker, Logger log, RollDisplayer rollDisplayer);
  void operator()(ShipPtr target);

private:
  int roll();
  OneGunRoll rollGuns(const ShipSpec& ship, int ShipSpec::*gun);
  AttackRoll attack(const ShipSpec& ship);
  std::function<HitResult(const OneGunRoll&)> resultOfAttackPart(int computer, int shield);
  ResultOfRoll resultOfAttack(const ShipSpec& shooter, const AttackRoll& roll, const ShipSpec& target);
  void applyDamagePerGun(OneGunRoll& oneGun, const HitResult& result, ShipPtr target, int damagePerHit);

  ShipPtr attacker_;
  AttackRoll roll_;
  RollDisplayer rollDisplayer_;
};

class DamageApplicationStrategy
{
public:
  virtual ~DamageApplicationStrategy() {}
  virtual ShipVector orderShips(const ShipVector& ships) = 0;
};

void apply_damage(ShipVector ships, DamageApplier& da, DamageApplicationStrategy& strategy);

class AncientsDamageApplicationStrategy : public DamageApplicationStrategy
{
public:
  virtual ShipVector orderShips(const ShipVector& ships);
};

}

#endif
