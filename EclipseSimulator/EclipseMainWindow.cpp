#include <EclipseMainWindow.hpp>
#include <ShipSpecEditorDialog.hpp>
#include <ShipGraphicsManager.hpp>
#include <ShipWidgetController.hpp>

#include <QDebug>
#include <QGraphicsItem>
#include <QtWidgets>
#include <Simulation.hpp>

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

  nextPushButton_->setEnabled(false);
  finishPushButton_->setEnabled(false);
}

void EclipseMainWindow::startBattle()
{
  startPushButton_->setEnabled(false);
  nextPushButton_->setEnabled(true);
  finishPushButton_->setEnabled(true);

  using namespace Simulation;

  AttackingFleet player { { "player" }, {} };
  DefendingFleet ancient { { "ancients" }, {}};
  for (const auto& ship : ships_)
  {
    if (ship->isAttacker())
      player.addNewShip(ship->spec(), ship->activeCount());
    else
      ancient.addNewShip(ship->spec(), ship->activeCount());
  }

  BattleHelper bh;
  bh.simulateBattle(player, ancient, numTrialsSpinBox_->value());
}

void EclipseMainWindow::incrementBattle()
{
  //auto r = Simulation::roll();
  //log(toString(r));
  //startPushButton_->setEnabled(false);
  //nextPushButton_->setEnabled(true);
  //finishPushButton_->setEnabled(true);
}

void EclipseMainWindow::finishBattle()
{
  log(__FUNCTION__);
  startPushButton_->setEnabled(true);
  nextPushButton_->setEnabled(false);
  finishPushButton_->setEnabled(false);
}

// void EclipseMainWindow::append(const QString& str)
// {
//   auto current = battleResultsTextEdit_->toPlainText();
//   battleResultsTextEdit_->setText(current + str);
//   battleResultsTextEdit_->verticalScrollBar()->setValue(battleResultsTextEdit_->verticalScrollBar()->maximum());
// }

void EclipseMainWindow::log(const QString& str)
{
  auto current = battleResultsTextEdit_->toPlainText();
  if (!current.isEmpty())
    current += "\n";
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
  std::transform(ships_.begin(), ships_.end(), std::back_inserter(shipNames), [](ShipWidgetController* swc) { return swc->name(); });
  shipGraphics_->addShipDescriptions(shipNames);
  shipGraphics_->addShipBorders();
  shipGraphics_->reorderShips();

  for (auto& shipWidget : ships_)
  {
    connect(shipWidget, &ShipWidgetController::shipAdded, shipGraphics_, &ShipGraphicsManager::addShipRect);
    connect(shipWidget, &ShipWidgetController::shipRemoved, shipGraphics_, &ShipGraphicsManager::removeShipRect);
    connect(shipWidget, &ShipWidgetController::initiativeChanged, shipGraphics_, &ShipGraphicsManager::adjustInitiative);
  }
}
