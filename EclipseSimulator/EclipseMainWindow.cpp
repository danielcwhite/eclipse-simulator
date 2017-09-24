#include <EclipseMainWindow.hpp>
#include <QDebug>

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
}

ShipWidgetController::ShipWidgetController(ShipWidgets widgets, const QString& name,
  int maxShips, QObject* parent) : QObject(parent), widgets_(widgets), name_(name),
  maxShips_(maxShips)
{
  qDebug() << "Setting up" << name_ << "widgets";

  connect(widgets_.add, &QToolButton::clicked, this, &ShipWidgetController::addShipPressed);
  connect(widgets_.remove, &QToolButton::clicked, this, &ShipWidgetController::removeShipPressed);
  connect(widgets_.edit, &QPushButton::clicked, this, &ShipWidgetController::editShipPressed);

  widgets_.remove->setDisabled(true);
}

void ShipWidgetController::addShipPressed()
{
  auto oldValue = widgets_.count->intValue();
  if (oldValue < maxShips_)
  {
    widgets_.count->display(oldValue + 1);
    if (oldValue + 1 == maxShips_)
    {
      widgets_.add->setDisabled(true);
    }
    widgets_.remove->setEnabled(true);
  }
}

void ShipWidgetController::removeShipPressed()
{
  auto oldValue = widgets_.count->intValue();
  if (oldValue > 0)
  {
    widgets_.count->display(oldValue - 1);
    if (oldValue - 1 == 0)
    {
      widgets_.remove->setDisabled(true);
    }
    widgets_.add->setEnabled(true);
  }
}

void ShipWidgetController::editShipPressed()
{
  qDebug() << "display ship spec editor";
}
