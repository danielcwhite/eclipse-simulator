#ifndef GUI_SHIP_H__
#define GUI_SHIP_H__

#include <Simulation.hpp>
#include <QDebug>

class ShipGraphicsManager;

class GuiShipImpl
{
public:
  explicit GuiShipImpl(ShipGraphicsManager* sgm);
  void setActive(bool active, const QString& name, int index, const QString& desc);
  void applyDamage(int amount, const QString& name, int index);
  void setMatch(const QString& fromName, int fromIndex, const QString& toName, int toIndex);
private:
  ShipGraphicsManager* sgm_;
};

class GuiShip : public ShipInterface
{
public:
  GuiShip(const Simulation::FightingShip& ship, const std::string& name, int index) :
    impl_(ship), name_(name), index_(index) {}
  void setGuiImpl(std::shared_ptr<GuiShipImpl> gsi);

  std::string name() const override;
  int index() const override;
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
  std::string name_;
  int index_;
  std::shared_ptr<GuiShipImpl> guiImpl_;
  QString qName() const { return QString::fromStdString(name_); }
  QString qDescribe() const { return QString::fromStdString(describe()); }
};


#endif
