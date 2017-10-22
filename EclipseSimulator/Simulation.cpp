#include <Simulation.hpp>

using namespace Simulation;

int BattleHelper::roll()
{
  static std::random_device rd;  //Will be used to obtain a seed for the random number engine
  static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
  static std::uniform_int_distribution<> dis(1, 6);
  return dis(gen);
}

OneGunRoll BattleHelper::rollGuns(const ShipSpec& ship, int ShipSpec::*gun)
{
  OneGunRoll gunRoll;
  for (int i = 0; i < ship.*gun; ++i)
    gunRoll.push_back(roll());
  return gunRoll;
}

AttackRoll BattleHelper::attack(const ShipSpec& ship)
{
  return {
    rollGuns(ship, &ShipSpec::yellowGuns),
    rollGuns(ship, &ShipSpec::orangeGuns),
    rollGuns(ship, &ShipSpec::redGuns)
  };
}

std::function<HitResult(const OneGunRoll&)> BattleHelper::resultOfAttackPart(int computer, int shield)
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

ResultOfRoll BattleHelper::resultOfAttack(const ShipSpec& shooter, const AttackRoll& roll, const ShipSpec& target)
{
  auto attackFunc = resultOfAttackPart(shooter.computer, target.shield);
  return {
    attackFunc(roll.yellowDice),
    attackFunc(roll.orangeDice),
    attackFunc(roll.redDice)
  };
}

static BattleHelper impl_;

DamageApplier::DamageApplier(const FightingShip& attacker) : attacker_(attacker)
{
  roll_ = impl_.attack(attacker.spec());
  //print() << "  Roll: \t" << roll_;
}

void DamageApplier::operator()(FightingShip& target)
{
  if (attacker_.isFighting(target))
  {
    auto result = impl_.resultOfAttack(attacker_.spec(), roll_, target.spec());

    for (auto i = 0; i < result.yellowDice.size(); ++i)
    {
      if (result.yellowDice[i] && target.isAlive())
      {
        //print() << "\t hits.\n";
        target.applyDamage(1);
        //print() << "Target is now " << target << '\n';
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

  // print() << "Battle with these sorted ships:\n";
  // auto i = 1;
  // for (const auto& ship : allShips_)
  // {
  //   print() << i++ << "\t" << ship << "\n";
  // }
  // print() << '\n';
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

    //print() << i++;
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
      //print() << "\nBATTLE COMPLETE: victor is " << victorString_ << '\n';
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
    //print() << "Round " << roundCount_ << ":\n";
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

void BattleHelper::simulateBattle(AttackingFleet& attacker, DefendingFleet& defender, int trials)
{
  std::map<std::string, int> results;
  for (int i = 0; i < trials; ++i)
  {
    Battle battle(attacker, defender);
    auto result = battle.fightToDeath();
    results[result]++;
    //print() << result << " wins" << '\n';
  }

  // print() << "After " << trials << " simulations, the results are: \n";
  // for (const auto& result : results)
  // {
  //   print() << result.first << " won " << result.second << " times.\n";
  // }
}
