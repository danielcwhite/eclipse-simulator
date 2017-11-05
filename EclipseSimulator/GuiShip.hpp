#ifndef GUI_SHIP_H__
#define GUI_SHIP_H__

#include <Simulation.hpp>
#include <QObject>

class GuiShip : public QObject, public ShipInterface
{
  Q_OBJECT
public:
  explicit GuiShip(const Simulation::FightingShip& ship) : impl_(ship) {}

  bool isAttacker() const override
  {
    return impl_.isAttacker();
  }

  bool isFighting(const ShipInterface& other) const override
  {
    return impl_.isFighting(static_cast<const GuiShip&>(other).impl_);
  }

  bool isAlive() const override
  {
    return impl_.isAlive();
  }

  void applyDamage(int amount) override
  {
    impl_.applyDamage(amount);
  }

  ShipSpec spec() const override
  {
    return impl_.spec();
  }

  std::string describe() const override
  {
    std::ostringstream ostr;
    ostr << impl_;
    return ostr.str();
  }
private:
  Simulation::FightingShip impl_;
  // todo: signals
};


#endif
