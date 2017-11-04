#include <Simulation.hpp>

using namespace Simulation;

BattleHelper::BattleHelper(Logger log) : HasLogger(log)
{
  log("BattleHelper initialized!");
}

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

AttackRoll DamageApplier::attack(const ShipSpec& ship)
{
  return {
    rollGuns(ship, &ShipSpec::yellowGuns),
    rollGuns(ship, &ShipSpec::orangeGuns),
    rollGuns(ship, &ShipSpec::redGuns)
  };
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

DamageApplier::DamageApplier(const ShipPtr& attacker, Logger l) : HasLogger(l), attacker_(attacker)
{
  roll_ = attack(attacker->spec());
  log(attacker->describe(), "\nrolls: \t", roll_);
}

void DamageApplier::operator()(ShipPtr target)
{
  if (attacker_->isFighting(*target))
  {
    if (roll_.yellowDice.empty())
    {
      log("All dice applied or 1s, nothing to do");
      return;
    }
    auto result = resultOfAttack(attacker_->spec(), roll_, target->spec());
    log("Result of roll against target: ", target->describe(), "\n", result);
    if (!std::any_of(result.yellowDice.begin(), result.yellowDice.end(), [](bool b) { return b; }))
    {
      log("All misses, checking next ship!");
      return;
    }
    //TODO: incorporate orange and red guns here--also abstract applying damage/using up dice
    for (auto i = 0; i < result.yellowDice.size(); ++i)
    {
      if (result.yellowDice[i] && target->isAlive())
      {
        log("\t hits: ", target->describe());
        target->applyDamage(1);
        log("Target status is now ", target->describe());
        roll_.yellowDice[i] = 1;
      }
      // else if (target->isAlive())
      // {
      //   log("\t misses: ", target->describe());
      // }
    }
    roll_.yellowDice.erase(std::remove(roll_.yellowDice.begin(), roll_.yellowDice.end(), 1),
      roll_.yellowDice.end());
  }
}

/*
Battle::Battle(const AttackingFleet& attacker, const DefendingFleet& defender, Logger l) : HasLogger(l)
{
  if (attacker.ships().empty() || defender.ships().empty())
  {
    log("No battle--empty side.");
    return;
  }
  std::copy(defender.ships().begin(), defender.ships().end(), std::back_inserter(allShips_));
  std::copy(attacker.ships().begin(), attacker.ships().end(), std::back_inserter(allShips_));
  std::sort(allShips_.begin(), allShips_.end());

  log("Battle with these sorted ships:");
  auto i = 1;
  for (const auto& ship : allShips_)
  {
    log(i++, "\t", ship);
  }
  log();
}

bool Battle::oneRound()
{
  if (battleComplete())
    return false;

  while (firedShips_.size() < allShips_.size())
  {
    auto attacker = allShips_.front();
    allShips_.pop_front();

    DamageApplier applyDamage(attacker, logger());
    std::for_each(allShips_.begin(), allShips_.end(), applyDamage);

    auto deadShipCleanup = [](const FightingShip& ship) { return !ship.isAlive(); };
    allShips_.erase(std::remove_if(allShips_.begin(), allShips_.end(), deadShipCleanup), allShips_.end());

    firedShips_.push_back(attacker);
    allShips_.push_back(attacker);

    if (battleComplete())
    {
      log("\nBATTLE COMPLETE: victor is ", victorString_);
      return false;
    }
  }
  firedShips_.clear();

  return true;
}

std::string Battle::fightToDeath()
{
  do
  {
    roundCount_++;
    log("Round ", roundCount_, ":");
  }
  while (oneRound());

  return victorString_;
}

bool Battle::battleComplete()
{
  if (allShips_.empty())
  {
    victorString_ = "Nobody";
    return true;
  }
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
*/
/*
void BattleHelper::simulateBattle(AttackingFleet& attacker, DefendingFleet& defender, int trials)
{
  std::map<std::string, int> results;
  for (int i = 0; i < trials; ++i)
  {
    Battle battle(attacker, defender, logger());
    auto result = battle.fightToDeath();
    results[result]++;
    log(result, " wins.");
  }

  log("After ", trials, " simulations, the results are:");
  for (const auto& result : results)
  {
    log(result.first, " won ", result.second, " times.");
  }
}
*/
