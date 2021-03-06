#include <EclipseMainWindow.hpp>
#include <ShipSpecEditorDialog.hpp>
#include <ShipGraphicsManager.hpp>
#include <ShipWidgetController.hpp>
#include <GuiShip.hpp>

#include <memory>
#include <QDebug>
#include <QGraphicsItem>
#include <QtWidgets>
#include <BattleStateMachine.hpp>

EclipseMainWindow::EclipseMainWindow()
{
  setupUi(this);

  ships_= {
    new ShipWidgetController(
      { attackerInterceptorDescriptionLabel_,
        attackerInterceptorEditPushButton_,
        attackerInterceptorAddToolButton_,
        attackerInterceptorRemoveToolButton_,
        attackerInterceptorCountLcdNumber_,
        groupBox_6
      },
      "Attacker Interceptor",
      8,
      this),
    new ShipWidgetController(
      { attackerCruiserDescriptionLabel_,
        attackerCruiserEditPushButton_,
        attackerCruiserAddToolButton_,
        attackerCruiserRemoveToolButton_,
        attackerCruiserCountLcdNumber_,
        groupBox_7
      },
      "Attacker Cruiser",
      4,
      this),
    new ShipWidgetController(
      { attackerDreadnoughtDescriptionLabel_,
        attackerDreadnoughtEditPushButton_,
        attackerDreadnoughtAddToolButton_,
        attackerDreadnoughtRemoveToolButton_,
        attackerDreadnoughtCountLcdNumber_,
        groupBox_8
      },
      "Attacker Dreadnought",
      2,
      this),

    new ShipWidgetController(
      { defenderInterceptorDescriptionLabel_,
        defenderInterceptorEditPushButton_,
        defenderInterceptorAddToolButton_,
        defenderInterceptorRemoveToolButton_,
        defenderInterceptorCountLcdNumber_,
        groupBox_10
      },
      "Defender Interceptor",
      8,
      this),
    new ShipWidgetController(
      { defenderCruiserDescriptionLabel_,
        defenderCruiserEditPushButton_,
        defenderCruiserAddToolButton_,
        defenderCruiserRemoveToolButton_,
        defenderCruiserCountLcdNumber_,
        groupBox_12
      },
      "Defender Cruiser",
      4,
      this),
    new ShipWidgetController(
      { defenderDreadnoughtDescriptionLabel_,
        defenderDreadnoughtEditPushButton_,
        defenderDreadnoughtAddToolButton_,
        defenderDreadnoughtRemoveToolButton_,
        defenderDreadnoughtCountLcdNumber_,
        groupBox_11
      },
      "Defender Dreadnought",
      2,
      this),
    new ShipWidgetController(
      { defenderStarbaseDescriptionLabel_,
        defenderStarbaseEditPushButton_,
        defenderStarbaseAddToolButton_,
        defenderStarbaseRemoveToolButton_,
        defenderStarbaseCountLcdNumber_,
        groupBox_13
      },
      "Defender Starbase",
      4,
      this)
  };

  setupBattleOrderView();

  connect(startPushButton_, &QPushButton::clicked, this, &EclipseMainWindow::startBattle);
  connect(nextPushButton_, &QPushButton::clicked, this, &EclipseMainWindow::incrementBattle);
  connect(finishPushButton_, &QPushButton::clicked, this, &EclipseMainWindow::finishBattle);
  connect(runSimulationPushButton_, &QPushButton::clicked, this, &EclipseMainWindow::simulateBattle);

  nextPushButton_->setEnabled(false);
  finishPushButton_->setEnabled(false);
}


class GuiShipFactory : public ShipFactory
{
public:
  explicit GuiShipFactory(ShipGraphicsManager* sgm) : sgm_(sgm) {}

  ShipPtr make(const std::tuple<Simulation::FightingShip, std::string, int>& namedShip) const override
  {
    auto guiShip = std::make_shared<GuiShip>(std::get<0>(namedShip),
      std::get<1>(namedShip), std::get<2>(namedShip));
    guiShip->setGuiImpl(std::make_shared<GuiShipImpl>(sgm_));

    return guiShip;
  }
private:
  ShipGraphicsManager* sgm_;
};

void EclipseMainWindow::startBattle()
{
  startPushButton_->setEnabled(false);
  nextPushButton_->setEnabled(true);
  finishPushButton_->setEnabled(true);

  for (auto& swc : ships_)
  {
    swc->setEnabled(false);
    if (swc->activeCount() == 0)
      swc->hide();
  }

  log("Start battle.\n");

  using namespace Simulation;
  using namespace StateMachine;

  AttackingFleet attacker { { "attacker" }, {} };
  DefendingFleet defender { { "defender" }, {} };
  for (const auto& ship : ships_)
  {
    if (ship->isAttacker())
      attacker.addNewShip(ship->name().toStdString(), ship->spec(), ship->activeCount());
    else
      defender.addNewShip(ship->name().toStdString(), ship->spec(), ship->activeCount());
  }
  static ShipFactoryPtr factory = std::make_shared<GuiShipFactory>(shipGraphics_);
  auto verboseLog = [this](const std::string& str)
  {
    if (verboseMode)
    {
      log(QString::fromStdString(str));
      std::cout << str;
    }
  };
  battle_ = std::make_shared<Battle2>(attacker, defender, factory, verboseLog,
    [this](const AttackRoll& roll) { displayRoll(roll); });

  std::map<QString, int> hitpoints;
  for (const auto& ship : ships_)
  {
    hitpoints[ship->name()] = ship->spec().hull + 1;
  }
  shipGraphics_->setShipHitpoints(hitpoints);
}

void EclipseMainWindow::displayRoll(const Simulation::AttackRoll& roll)
{
  if (is_empty(roll))
  {
    rollLabel_->setText("");
    return;
  }
  
  std::ostringstream display;
  display << "Last roll: ";
  for (const auto& y : roll.yellowDice)
  {
    display << "<font color=yellow>" << y << ",</font>";
  }
  for (const auto& y : roll.orangeDice)
  {
    display << "<font color=orange>" << y << ",</font>";
  }
  for (const auto& y : roll.redDice)
  {
    display << "<font color=red>" << y << ",</font>";
  }
  rollLabel_->setText(QString::fromStdString(display.str()));
}

void EclipseMainWindow::incrementBattle()
{
  if (battle_)
  {
    if (verboseMode)
      log("--------------------------\n");
    auto result = battle_->update();

    nextPushButton_->setEnabled(result);
  }
}

void EclipseMainWindow::finishBattle()
{
  log("finishBattle\n");
  startPushButton_->setEnabled(true);
  nextPushButton_->setEnabled(false);
  finishPushButton_->setEnabled(false);
  for (auto& swc : ships_)
  {
    swc->setEnabled(true);
    if (swc->activeCount() == 0)
      swc->show();
  }
  shipGraphics_->clearDamage();
  shipGraphics_->setShipHitpoints({});
  battle_.reset();
}

void EclipseMainWindow::simulateBattle()
{
  log("Does nothing yet\n");
}

void EclipseMainWindow::log(const QString& str)
{
  auto current = battleResultsTextEdit_->toPlainText();
  battleResultsTextEdit_->setText(current + str);
  battleResultsTextEdit_->verticalScrollBar()->setValue(battleResultsTextEdit_->verticalScrollBar()->maximum());
}

void EclipseMainWindow::setupBattleOrderView()
{
  scene_ = new QGraphicsScene(this);
  battleOrderGraphicsView_->setScene(scene_);
  scene_->setBackgroundBrush(Qt::black);

  shipGraphics_ = new ShipGraphicsManager(scene_, this);
  std::vector<QString> shipNames;
  std::transform(ships_.begin(), ships_.end(), std::back_inserter(shipNames),
    [](ShipWidgetController* swc) { return swc->name(); });
  shipGraphics_->addShipDescriptions(shipNames);
  std::map<QString, int> maxShips;
  std::transform(ships_.begin(), ships_.end(), std::inserter(maxShips, maxShips.end()),
    [](ShipWidgetController* swc) { return std::make_pair(swc->name(), swc->maximum()); });
  shipGraphics_->addShipBorders(maxShips);
  shipGraphics_->reorderShips();

  for (auto& shipWidget : ships_)
  {
    connect(shipWidget, &ShipWidgetController::shipAdded, shipGraphics_, &ShipGraphicsManager::addShipRect);
    connect(shipWidget, &ShipWidgetController::shipRemoved, shipGraphics_, &ShipGraphicsManager::removeShipRect);
    connect(shipWidget, &ShipWidgetController::initiativeChanged, shipGraphics_, &ShipGraphicsManager::adjustInitiative);
    shipWidget->updateShipCount();
    shipWidget->readSpec();
  }
}
