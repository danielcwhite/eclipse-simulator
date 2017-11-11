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
      ShipFactoryPtr factory,
      Simulation::Logger log,
      Simulation::RollDisplayer rollDisplayer
    );
    ~Battle2();

    bool update();

    void setActiveAttacker();
    void applyDamage();
    bool roundComplete() const;
    void cleanupDeadShips();
    bool checkForVictory();
    void resetShipsForNextRound();

    std::string victor() const { return victorString_; }

  private:
    std::shared_ptr<BattleState> state_;

    ShipPtr activeAttacker_;
    std::deque<ShipPtr> allShips_, firedShips_;
    int roundCount_{0};
    std::string victorString_;
    Simulation::RollDisplayer rollDisplayer_;

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
