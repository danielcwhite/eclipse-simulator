#include <Simulation.hpp>

DamageApplier::DamageApplier(const FightingShip& attacker) : attacker_(attacker)
{
  roll_ = attack(attacker.spec());
  print() << "  Roll: \t" << roll_;
}
void DamageApplier::operator()(FightingShip& target)
{
  if (attacker_.isFighting(target))
  {
    auto result = resultOfAttack(attacker_.spec(), roll_, target.spec());

    for (auto i = 0; i < result.yellowDice.size(); ++i)
    {
      if (result.yellowDice[i] && target.isAlive())
      {
        print() << "\t hits.\n";
        target.applyDamage(1);
        print() << "Target is now " << target << '\n';
        roll_.yellowDice[i] = 1;
      }
    }
  }
}

Battle::Battle(const AttackingFleet& attacker, const DefendingFleet& defender)
{
  std::copy(defender.ships().begin(), defender.ships().end(), std::back_inserter(allShips_));
  std::copy(attacker.ships().begin(), attacker.ships().end(), std::back_inserter(allShips_));
  std::sort(allShips_.begin(), allShips_.end());

  print() << "Battle with these sorted ships:\n";
  auto i = 1;
  for (const auto& ship : allShips_)
  {
    print() << i++ << "\t" << ship << "\n";
  }
  print() << '\n';
}

bool Battle::oneRound()
{
  if (battleComplete())
    return false;

  auto i = 1;
  while (firedShips_.size() < allShips_.size())
  {
    auto attacker = allShips_.front();
    allShips_.pop_front();

    print() << i++;
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
      print() << "\nBATTLE COMPLETE: victor is " << victorString_ << '\n';
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
    print() << "Round " << roundCount_ << ":\n";
  }
  while (oneRound());

  return victorString_;
}

bool Battle::battleComplete()
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

void simulateBattle(AttackingFleet& attacker, DefendingFleet& defender, int trials)
{
  std::map<std::string, int> results;
  for (int i = 0; i < trials; ++i)
  {
    Battle battle(attacker, defender);
    auto result = battle.fightToDeath();
    results[result]++;
    print() << result << " wins" << '\n';
  }

  print() << "After " << trials << " simulations, the results are: \n";
  for (const auto& result : results)
  {
    print() << result.first << " won " << result.second << " times.\n";
  }
}
