#include <Simulation.hpp>

using namespace Simulation;

BattleHelper::BattleHelper(Logger log) : HasLogger(log)
{
  log("BattleHelper initialized!");
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
