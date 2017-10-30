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
}

void Battle2::applyDamage()
{
  log(__PRETTY_FUNCTION__);
}

void Battle2::cleanupDeadShips()
{
  log(__PRETTY_FUNCTION__);
}

bool Battle2::checkForVictory()
{
  log(__PRETTY_FUNCTION__);
  return false;
}

void Battle2::resetShipsForNextRound()
{
  log(__PRETTY_FUNCTION__);
}
