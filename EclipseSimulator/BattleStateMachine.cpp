#include <BattleStateMachine.hpp>

using namespace StateMachine;

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
  return std::make_shared<CleanupDeadShipsState>();
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
