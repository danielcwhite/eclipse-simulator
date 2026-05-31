#include <DamageApplier.hpp>

using namespace Simulation;

int DamageApplier::roll()
{
  static std::random_device rd;  //Will be used to obtain a seed for the random number engine
  static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
  static std::uniform_int_distribution<> dis(1, 6);
  return dis(gen);
}

OneGunRoll DamageApplier::rollGuns(const ShipSpec& ship, int ShipSpec::*gun)
{
  OneGunRoll gunRoll;
  for (int i = 0; i < ship.*gun; ++i)
    gunRoll.push_back(roll());
  return gunRoll;
}

AttackRoll DamageApplier::cannonRoll(const ShipSpec& ship)
{
  return {
    rollGuns(ship, &ShipSpec::yellowGuns),
    rollGuns(ship, &ShipSpec::orangeGuns),
    rollGuns(ship, &ShipSpec::redGuns)
  };
}

AttackRoll DamageApplier::missileRoll(const ShipSpec& ship)
{
  // Each missile part fires 2 orange dice; no yellow or red dice
  OneGunRoll orangeDice;
  for (int i = 0; i < ship.missiles * 2; ++i)
    orangeDice.push_back(roll());
  return { {}, orangeDice, {} };
}

DamageApplier::DamageApplier(const ShipPtr& attacker, AttackRoll roll, Logger l, RollDisplayer rollDisplayer) :
  HasLogger(l), attacker_(attacker), roll_(std::move(roll)), rollDisplayer_(rollDisplayer)
{
  log(attacker->toString(), " rolls: \t", roll_);
  rollDisplayer_(roll_);
}

DamageApplier DamageApplier::makeCannonApplier(const ShipPtr& attacker, Logger log, RollDisplayer rollDisplayer)
{
  return DamageApplier(attacker, cannonRoll(attacker->spec()), log, rollDisplayer);
}

DamageApplier DamageApplier::makeMissileApplier(const ShipPtr& attacker, Logger log, RollDisplayer rollDisplayer)
{
  return DamageApplier(attacker, missileRoll(attacker->spec()), log, rollDisplayer);
}

std::function<HitResult(const OneGunRoll&)> DamageApplier::resultOfAttackPart(int computer, int shield)
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

ResultOfRoll DamageApplier::resultOfAttack(const ShipSpec& shooter, const AttackRoll& roll, const ShipSpec& target)
{
  auto attackFunc = resultOfAttackPart(shooter.computer, target.shield);
  return {
    attackFunc(roll.yellowDice),
    attackFunc(roll.orangeDice),
    attackFunc(roll.redDice)
  };
}

void DamageApplier::operator()(ShipPtr target)
{
  if (attacker_->isFighting(*target))
  {
    auto result = resultOfAttack(attacker_->spec(), roll_, target->spec());
    if (!is_empty(result))
    {
      log("Vs target: ", target->toString(), "\t", result);
    //TODO: beautify
      applyDamagePerGun(roll_.yellowDice, result.yellowDice, target, 1);
      applyDamagePerGun(roll_.orangeDice, result.orangeDice, target, 2);
      applyDamagePerGun(roll_.redDice, result.redDice, target, 4);
    }
  }
}

void DamageApplier::applyDamagePerGun(OneGunRoll& oneGun, const HitResult& result, ShipPtr target, int damagePerHit)
{
  if (oneGun.empty())
  {
    //log("All dice applied or 1s, nothing to do");
    return;
  }

  if (!std::any_of(result.begin(), result.end(), [](bool b) { return b; }))
  {
    log("All misses, checking next ship!");
    return;
  }
  //TODO: incorporate orange and red guns here--also abstract applying damage/using up dice
  for (auto i = 0; i < result.size(); ++i)
  {
    if (result[i] && target->isAlive())
    {
      log("\t hits: ", target->toString());
      target->applyDamage(damagePerHit);
      log("Target status is now ", target->describe());
      oneGun[i] = 1;
    }
  }
  oneGun.erase(std::remove(oneGun.begin(), oneGun.end(), 1), oneGun.end());
}

void Simulation::apply_damage(ShipVector ships, DamageApplier& da, DamageApplicationStrategy& strategy)
{
  auto sorted = strategy.orderShips(ships);
  std::for_each(sorted.begin(), sorted.end(), da);
}

// Ship type priority for ancient damage targeting: prefer to kill smallest first,
// then by type (Interceptor < Cruiser < Dreadnought).
static int shipTypePriority(const std::string& name)
{
  if (name.find("Interceptor") != std::string::npos) return 0;
  if (name.find("Cruiser")     != std::string::npos) return 1;
  if (name.find("Starbase")    != std::string::npos) return 2;
  if (name.find("Dreadnought") != std::string::npos) return 3;
  return 1; // unknown — treat as Cruiser
}

ShipVector AncientsDamageApplicationStrategy::orderShips(const ShipVector& ships)
{
  auto copy(ships);
  std::sort(copy.begin(), copy.end(), [](const ShipPtr& lhs, const ShipPtr& rhs)
  {
    if (lhs->spec().hull != rhs->spec().hull)
      return lhs->spec().hull < rhs->spec().hull;
    return shipTypePriority(lhs->name()) < shipTypePriority(rhs->name());
  });
  return copy;
}
