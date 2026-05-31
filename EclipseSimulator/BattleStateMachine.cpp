#include <BattleStateMachine.hpp>
#include <DamageApplier.hpp>

using namespace StateMachine;
using namespace Simulation;

template <class T>
bool operator<(const T& lhs, const T& rhs)
{
  return lhs.lessThan(rhs);
}

template <class T>
struct PtrSort
{
  bool operator()(const std::shared_ptr<T>& t1, const std::shared_ptr<T>& t2) const
  {
    return *t1 < *t2;
  }
};

Battle2::Battle2(const AttackingFleet& attacker, const DefendingFleet& defender,
  ShipFactoryPtr factory,
  Logger l, RollDisplayer rd)
  : HasLogger(l), rollDisplayer_(rd)
{
  if (attacker.ships().empty() || defender.ships().empty())
  {
    log("No battle--empty side.");
    return;
  }

  for (const auto& def : defender.ships())
    allShips_.push_back(factory->make(def));
  for (const auto& atk : attacker.ships())
    allShips_.push_back(factory->make(atk));
  std::sort(allShips_.begin(), allShips_.end(), PtrSort<ShipInterface>());

  log("Battle with these sorted ships:");
  auto i = 1;
  for (const auto& ship : allShips_)
    log(i++, "\t", ship->toString(), "\t", ship->describe());
  log();

  // Populate missile queue: ships with missiles, in initiative order (already sorted)
  for (const auto& ship : allShips_)
    if (ship->spec().missiles > 0)
      missileShips_.push_back(ship);

  if (!missileShips_.empty())
    log("Missile phase: ", missileShips_.size(), " ship(s) will fire missiles.");

  state_ = std::make_shared<MissilePickShooterState>();
}

Battle2::~Battle2()
{
  if (activeAttacker_)
    activeAttacker_->setActive(false);
}

bool Battle2::update()
{
  if (!state_)
    return false;
  state_ = state_->update(*this);
  return state_ != nullptr;
}

// --- Missile phase ---

std::shared_ptr<BattleState> MissilePickShooterState::update(Battle2& battle)
{
  // Skip dead ships in missile queue, then check if any remain
  battle.setActiveMissileShooter();
  if (!battle.missileShipsRemaining())
  {
    // No missiles left to fire — begin engagement rounds
    return std::make_shared<PickActiveAttackerState>();
  }
  return std::make_shared<MissileFireState>();
}

std::shared_ptr<BattleState> MissileFireState::update(Battle2& battle)
{
  battle.fireMissiles();
  return std::make_shared<MissileCleanupDeadShipsState>();
}

std::shared_ptr<BattleState> MissileCleanupDeadShipsState::update(Battle2& battle)
{
  battle.cleanupDeadShips();
  return std::make_shared<MissileCheckVictoryState>();
}

std::shared_ptr<BattleState> MissileCheckVictoryState::update(Battle2& battle)
{
  if (battle.checkForVictory())
    return nullptr;
  return std::make_shared<MissilePickShooterState>();
}

void Battle2::setActiveMissileShooter()
{
  // Drain dead or already-destroyed ships from the front of the missile queue
  while (!missileShips_.empty() && !missileShips_.front()->isAlive())
    missileShips_.pop_front();

  if (missileShips_.empty())
  {
    activeAttacker_ = nullptr;
    return;
  }

  activeAttacker_ = missileShips_.front();
  missileShips_.pop_front();
  log("Missile fire: ", activeAttacker_->toString());
  activeAttacker_->setActive(true);
  rollDisplayer_({});
}

bool Battle2::missileShipsRemaining() const
{
  return activeAttacker_ != nullptr;
}

void Battle2::fireMissiles()
{
  auto da = DamageApplier::makeMissileApplier(activeAttacker_, logger(), rollDisplayer_);
  AncientsDamageApplicationStrategy ancients;
  apply_damage(allShips_, da, ancients);
}

// Missile cleanupDeadShips is the same as engagement, but doesn't push activeAttacker_
// back into allShips_ (missiles fire once and are done).
// We reuse cleanupDeadShips() but override its end behavior via a flag — instead,
// just inline a simpler version here by calling the shared logic and handling attacker ourselves.

// --- Engagement round states ---

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
  activeAttacker_ = allShips_.front();
  log("Active ship: ", activeAttacker_->toString());
  activeAttacker_->setActive(true);
  allShips_.pop_front();
  rollDisplayer_({});
}

void Battle2::applyDamage()
{
  auto da = DamageApplier::makeCannonApplier(activeAttacker_, logger(), rollDisplayer_);
  AncientsDamageApplicationStrategy ancients; // todo: user input
  apply_damage(allShips_, da, ancients);
}

bool Battle2::roundComplete() const
{
  auto complete = firedShips_.size() == allShips_.size();
  if (complete)
    log("round is complete.");
  else
    log("#ships still left to fire: ", allShips_.size() - firedShips_.size(), " (",
      allShips_.size(), "-", firedShips_.size(), ")");
  return complete;
}

void Battle2::cleanupDeadShips()
{
  auto deadShipCleanup = [](ShipPtr& ship)
  {
    auto isDead = !ship->isAlive();
    if (isDead)
      ship->setAsDead();
    return isDead;
  };
  auto count = allShips_.size();
  allShips_.erase(std::remove_if(allShips_.begin(), allShips_.end(), deadShipCleanup), allShips_.end());
  firedShips_.erase(std::remove_if(firedShips_.begin(), firedShips_.end(), deadShipCleanup), firedShips_.end());
  auto removed = count - allShips_.size();
  log("Removed ", removed, " dead ships.");

  if (activeAttacker_)
  {
    firedShips_.push_back(activeAttacker_);
    allShips_.push_back(activeAttacker_);
    activeAttacker_->setActive(false);
    activeAttacker_ = nullptr;
  }
}

bool Battle2::checkForVictory()
{
  if (battleComplete())
  {
    log("\nBATTLE COMPLETE: victor is ", victorString_, "\n");
    return true;
  }
  else
    log("Battle is ongoing.");

  return false;
}

bool Battle2::battleComplete()
{
  if (allShips_.empty())
  {
    victorString_ = "Nobody";
    return true;
  }
  if (std::all_of(allShips_.begin(), allShips_.end(), [](const ShipPtr& ship) { return ship->isAttacker(); }))
  {
    victorString_ = "Attacker";
    return true;
  }
  if (std::all_of(allShips_.begin(), allShips_.end(), [](const ShipPtr& ship) { return !ship->isAttacker(); }))
  {
    victorString_ = "Defender";
    return true;
  }
  if (roundCount_ > 100)
  {
    victorString_ = "Stalemate";
    return true;
  }
  return false;
}

void Battle2::resetShipsForNextRound()
{
  roundCount_++;
  log("Ready for next round ", roundCount_, ".");
  firedShips_.clear();
}
