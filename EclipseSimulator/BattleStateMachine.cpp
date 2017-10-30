#include <BattleStateMachine.hpp>

using namespace StateMachine;
using namespace Simulation;

Battle2::Battle2(const AttackingFleet& attacker, const DefendingFleet& defender, Logger l)
  : HasLogger(l)
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

  state_ = std::make_shared<PickActiveAttackerState>();
}

bool Battle2::update()
{
  state_ = state_->update(*this);
  return state_ != nullptr;
}

std::shared_ptr<BattleState> PickActiveAttackerState::update(Battle2& battle)
{
  battle.setActiveAttacker();
  return std::make_shared<ApplyDamageState>();
}

std::shared_ptr<BattleState> ApplyDamageState::update(Battle2& battle)
{
  battle.applyDamage();
  return std::make_shared<CleanUpDeadShipsState>();
}

std::shared_ptr<BattleState> CleanUpDeadShipsState::update(Battle2& battle)
{
  battle.cleanupDeadShips();
  return std::make_shared<CheckForVictoryState>();
}

std::shared_ptr<BattleState> CheckForVictoryState::update(Battle2& battle)
{
  if (battle.checkForVictory())
    return nullptr;
  else if (!battle.roundComplete())
    return std::make_shared<PickActiveAttackerState>();
  else
    return std::make_shared<ResetShipsState>();
}

std::shared_ptr<BattleState> ResetShipsState::update(Battle2& battle)
{
  battle.resetShipsForNextRound();
  return std::make_shared<PickActiveAttackerState>();
}

void Battle2::setActiveAttacker()
{
  log(__PRETTY_FUNCTION__);
  attacker_.reset(new FightingShip(allShips_.front()));
  log("Active ship: ", *attacker_);
  allShips_.pop_front();
}

void Battle2::applyDamage()
{
  log(__PRETTY_FUNCTION__);
  DamageApplier applyDamage(*attacker_, logger());
  std::for_each(allShips_.begin(), allShips_.end(), applyDamage);
}

bool Battle2::roundComplete() const
{
  auto complete = firedShips_.size() == allShips_.size();
  if (complete)
    log("round is complete.");
  else
    log("#ships still left to fire: ", allShips_.size() - firedShips_.size());
  return complete;
}

void Battle2::cleanupDeadShips()
{
  log(__PRETTY_FUNCTION__);

  auto deadShipCleanup = [](const FightingShip& ship) { return !ship.isAlive(); };
  auto count = allShips_.size();
  allShips_.erase(std::remove_if(allShips_.begin(), allShips_.end(), deadShipCleanup), allShips_.end());
  auto removed = count - allShips_.size();
  log("Removed ", removed, " dead ships.");

  firedShips_.push_back(*attacker_);
  allShips_.push_back(*attacker_);
}

bool Battle2::checkForVictory()
{
  log(__PRETTY_FUNCTION__);

  if (battleComplete())
  {
    log("\nBATTLE COMPLETE: victor is ", victorString_, "\n");
    return true;
  }

  return false;
}

bool Battle2::battleComplete()
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

void Battle2::resetShipsForNextRound()
{
  log(__PRETTY_FUNCTION__);
  firedShips_.clear();
}
