#ifndef GUI_SHIP_H__
#define GUI_SHIP_H__

#include <Simulation.hpp>
#include <QDebug>

class ShipGraphicsManager;

class GuiShipImpl
{
public:
  explicit GuiShipImpl(ShipGraphicsManager* sgm);
  void setActive(bool active, const QString& desc);
private:
  ShipGraphicsManager* sgm_;
};

class GuiShip : public ShipInterface
{
public:
  explicit GuiShip(const Simulation::FightingShip& ship) : impl_(ship) {}
  void setGuiImpl(std::shared_ptr<GuiShipImpl> gsi);
  bool isAttacker() const override;
  bool isFighting(const ShipInterface& other) const override;
  bool isAlive() const override;
  void applyDamage(int amount) override;
  ShipSpec spec() const override;
  std::string describe() const override;
  bool lessThan(const ShipInterface& rhs) const override;
  void setActive(bool active) override;
private:
  Simulation::FightingShip impl_;
  std::shared_ptr<GuiShipImpl> guiImpl_;
  QString qDescribe() const { return QString::fromStdString(describe()); }
};


#endif
