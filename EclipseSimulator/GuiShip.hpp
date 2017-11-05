#ifndef GUI_SHIP_H__
#define GUI_SHIP_H__

#include <Simulation.hpp>
#include <QObject>
#include <QDebug>

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
    Q_EMIT damageApplied(amount, qDescribe());
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

  bool lessThan(const ShipInterface& rhs) const override
  {
   return impl_ < static_cast<const GuiShip&>(rhs).impl_;
  }

  void setActive(bool active) override
  {
    qDebug() << __PRETTY_FUNCTION__;
    Q_EMIT activeShipSet(active, qDescribe());
  }

Q_SIGNALS:
  void damageApplied(int damage, const QString& desc);
  void activeShipSet(bool active, const QString& desc);
private:
  Simulation::FightingShip impl_;
  QString qDescribe() const { return QString::fromStdString(describe()); }
};


#endif
