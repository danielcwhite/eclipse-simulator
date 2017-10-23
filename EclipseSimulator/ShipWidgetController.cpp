#include <ShipWidgetController.hpp>
#include <ShipSpecEditorDialog.hpp>
#include <QDebug>

ShipWidgetController::ShipWidgetController(ShipWidgets widgets, const QString& name,
  int maxShips, QWidget* parent) : QObject(parent), widgets_(widgets), name_(name),
  maxShips_(maxShips), editor_(new ShipSpecEditorDialog(name, parent))
{
  connect(widgets_.add, &QToolButton::clicked, this, &ShipWidgetController::addShipPressed);
  connect(widgets_.remove, &QToolButton::clicked, this, &ShipWidgetController::removeShipPressed);
  connect(widgets_.edit, &QPushButton::clicked, this, &ShipWidgetController::editShipPressed);

  widgets_.remove->setDisabled(true);

  editor_->hide();
  editor_->setSpec({});
  connect(editor_, &QDialog::accepted, this, &ShipWidgetController::specAccepted);
  connect(editor_, &QDialog::rejected, this, &ShipWidgetController::specRejected);

  updateSpecLabel();
}

void ShipWidgetController::setEnabled(bool enabled)
{
  widgets_.edit->setEnabled(enabled);
  widgets_.add->setEnabled(enabled);
  widgets_.remove->setEnabled(enabled);
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
    Q_EMIT shipAdded(name_, spec_.initiative);
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
    Q_EMIT shipRemoved(name_);
  }
}

void ShipWidgetController::editShipPressed()
{
  editor_->blockSignals(true);
  editor_->setSpec(spec_);
  editor_->blockSignals(false);
  editor_->show();
}

void ShipWidgetController::specAccepted()
{
  int oldInitiative = spec_.initiative;
  spec_ = editor_->spec();
  updateSpecLabel();
  if (oldInitiative != spec_.initiative)
    Q_EMIT initiativeChanged(name_, spec_.initiative);
}

void ShipWidgetController::updateSpecLabel()
{
  auto desc = tr("<i>^%1 *%2 +%3 -%4</i><p><b><font color=yellow>%5</font> <font color=orange>%6</font> <font color=red>%7</font></b>")
    .arg(spec_.initiative)
    .arg(spec_.hull)
    .arg(spec_.computer)
    .arg(spec_.shield)
    .arg(spec_.yellowGuns)
    .arg(spec_.orangeGuns)
    .arg(spec_.redGuns);
  widgets_.description->setText(desc);
}

void ShipWidgetController::specRejected()
{
  qDebug() << "spec rejected:" << name_;
}
