#ifndef INTERFACE_APPLICATION_SCIRUN_MAIN_WINDOW_H
#define INTERFACE_APPLICATION_SCIRUN_MAIN_WINDOW_H

#include <QMainWindow>
#include <ShipSpec.hpp>
#include <Simulation.hpp>
#include <sstream>
#include <memory>
#include "ui_EclipseSimulatorMainWindow.h"
#include "ui_ShipSpecEditor.h"

class ShipWidgetController;
class ShipGraphicsManager;

namespace StateMachine
{
  class Battle2;
}

class EclipseMainWindow : public QMainWindow, public Ui::EclipseSimMainWindow
{
  Q_OBJECT

public:
  EclipseMainWindow();

private Q_SLOTS:
  void startBattle();
  void incrementBattle();
  void finishBattle();
  void simulateBattle();

private:
  void setupBattleOrderView();
  void log(const QString& str);

  QGraphicsScene* scene_;
  std::vector<ShipWidgetController*> ships_;
  ShipGraphicsManager* shipGraphics_;
  std::shared_ptr<StateMachine::Battle2> battle_;
};

template <class T>
QString toString(const T& t)
{
  std::ostringstream o;
  o << t;
  return QString::fromStdString(o.str());
}

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
