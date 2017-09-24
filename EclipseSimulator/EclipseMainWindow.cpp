#include <EclipseMainWindow.hpp>
#include <QDebug>

EclipseMainWindow::EclipseMainWindow()
{
  setupUi(this);

  ships_.push_back(new ShipWidgetController(
    { attackerInterceptorDescriptionLabel_,
      attackerInterceptorEditPushButton_,
      attackerInterceptorAddToolButton_,
      attackerInterceptorRemoveToolButton_,
      attackerInterceptorCountLcdNumber_,
    },
    "Attacker Interceptor",
    8,
    this)
  );
}

ShipWidgetController::ShipWidgetController(ShipWidgets widgets, const QString& name,
  int maxShips, QObject* parent) : QObject(parent), widgets_(widgets), name_(name),
  maxShips_(maxShips)
{
  qDebug() << "Setting up" << name_ << "widgets";

  connect(widgets_.add, &QPushButton::clicked, this, &ShipWidgetController::addShipPressed);
  connect(widgets_.remove, &QPushButton::clicked, this, &ShipWidgetController::removeShipPressed);
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
