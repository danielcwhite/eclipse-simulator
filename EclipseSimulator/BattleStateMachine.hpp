#ifndef BATTLE_STATE_MACHINE
#define BATTLE_STATE_MACHINE

#include <Simulation.hpp>
#include <memory>

namespace StateMachine
{

  enum class State
  {
    PICK_ROLLER,
    ROLLING,
    APPLY_DAMAGE,
    CHECK_VICTORY
  };

  class Battle2;

  class BattleState
  {
  public:
    virtual ~BattleState() {}
    virtual std::shared_ptr<BattleState> update(Battle2& battle) = 0;
  };

  class Battle2 : public Simulation::HasLogger
  {
  public:
    Battle2(const Simulation::AttackingFleet& attacker,
      const Simulation::DefendingFleet& defender,
      Simulation::Logger log);

    bool update();

    void setActiveAttacker();
    void applyDamage();
    void cleanupDeadShips();
    bool checkForVictory();
    void resetShipsForNextRound();

    std::string victor() const { return victorString_; }

  private:
    std::shared_ptr<BattleState> state_;

    std::unique_ptr<Simulation::FightingShip> attacker_;
    std::deque<Simulation::FightingShip> allShips_, firedShips_;
    int roundCount_{0};
    std::string victorString_;

    bool battleComplete();
  };

  class PickActiveAttackerState : public BattleState
  {
  public:
    std::shared_ptr<BattleState> update(Battle2& battle) override;
  };

  class ApplyDamageState : public BattleState
  {
  public:
    std::shared_ptr<BattleState> update(Battle2& battle) override;
  };

  class CleanUpDeadShipsState : public BattleState
  {
  public:
    std::shared_ptr<BattleState> update(Battle2& battle) override;
  };

  class CheckForVictoryState : public BattleState
  {
  public:
    std::shared_ptr<BattleState> update(Battle2& battle) override;
  };

  class ResetShipsState : public BattleState
  {
  public:
    std::shared_ptr<BattleState> update(Battle2& battle) override;
  };


}

#endif
