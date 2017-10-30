#include <EclipseMainWindow.hpp>
#include <ShipSpecEditorDialog.hpp>
#include <ShipGraphicsManager.hpp>
#include <ShipWidgetController.hpp>

#include <QDebug>
#include <QGraphicsItem>
#include <QtWidgets>
#include <Simulation.hpp>
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

void EclipseMainWindow::startBattle()
{
  startPushButton_->setEnabled(false);
  nextPushButton_->setEnabled(true);
  finishPushButton_->setEnabled(true);

  for (auto& swc : ships_)
    swc->setEnabled(false);

  log("Start battle.\n");


  using namespace Simulation;
  using namespace StateMachine;

  AttackingFleet attacker { { "attacker" }, {} };
  DefendingFleet defender { { "defender" }, {} };
  for (const auto& ship : ships_)
  {
    if (ship->isAttacker())
      attacker.addNewShip(ship->spec(), ship->activeCount());
    else
      defender.addNewShip(ship->spec(), ship->activeCount());
  }
  battle_ = std::make_shared<Battle2>(attacker, defender,
    [this](const std::string& str) { log(QString::fromStdString(str)); });
}

void EclipseMainWindow::incrementBattle()
{
  if (battle_)
  {
    auto result = battle_->update();
    log(tr("result of incrementBattle: %0\n").arg(result));

    startPushButton_->setEnabled(!result);
    nextPushButton_->setEnabled(result);
    finishPushButton_->setEnabled(result);
  }
}

void EclipseMainWindow::finishBattle()
{
  log("finishBattle\n");
  startPushButton_->setEnabled(true);
  nextPushButton_->setEnabled(false);
  finishPushButton_->setEnabled(false);
  for (auto& swc : ships_)
    swc->setEnabled(true);
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
  }
}
