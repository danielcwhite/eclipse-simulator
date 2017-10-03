#include <EclipseMainWindow.hpp>
#include <QDebug>
#include <QGraphicsItem>

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
}

void EclipseMainWindow::setupBattleOrderView()
{
  scene_ = new QGraphicsScene(this);
  battleOrderGraphicsView_->setScene(scene_);
  scene_->setBackgroundBrush(Qt::black);

  shipGraphics_ = new ShipGraphicsManager(scene_, this);
  shipGraphics_->addShipBorders();

  for (auto& shipWidget : ships_)
  {
    connect(shipWidget, &ShipWidgetController::shipAdded, shipGraphics_, &ShipGraphicsManager::addShipRect);
    connect(shipWidget, &ShipWidgetController::shipRemoved, shipGraphics_, &ShipGraphicsManager::removeShipRect);
    connect(shipWidget, &ShipWidgetController::initiativeChanged, shipGraphics_, &ShipGraphicsManager::reorderShips);
  }

  //auto text = scene_->addText("I", QFont("Arial", 12) );
  //text->setPos(7, 7);

  //qDebug() << battleOrderGraphicsView_->mapToScene(battleOrderGraphicsView_->rect()).boundingRect();
}

ShipGraphicsManager::ShipGraphicsManager(QGraphicsScene* scene, QWidget* parent)
  : QObject(parent), scene_(scene)
{}

namespace
{
  const int w = 20;
  const int h = 30;
  const int spacing = 10;
  const int maxShipTypes = 7;
  const int maxShips = 8;
  const int border = 5;
}

void ShipGraphicsManager::addShipBorders()
{
  QBrush blueBrush(Qt::blue);
  QPen outlinePen(Qt::white);
  outlinePen.setWidth(2);

  for (int i = 0; i < maxShips; ++i)
  {
    for (int j = 0; j < maxShipTypes; ++j)
    {
      auto rectangle = scene_->addRect(border + i*(w + spacing), border + j*(h + spacing), w, h, outlinePen, Qt::black);
      rectItems_[j][i].item = rectangle;
    }
  }
}

/*
0 att int
1 att cru
2 att dre
3 def int
4 def cru
5 def dre
6 def stb
*/

void ShipGraphicsManager::addShipRect(const QString& name, int initiative)
{
  qDebug() << __FUNCTION__ << name << initiative;
  auto desc = name.split(' ');
  auto isAttacker = desc[0] == "Attacker";
  auto shipType = desc[1];
  auto color = isAttacker ? Qt::red : Qt::blue;
  auto leftSide = isAttacker;

  int firstShip = leftSide ? 0 : maxShips - 1;
  int shipIndex = -1;
  for (int j = 0; j < maxShipTypes; ++j)
  {
    if (rectItems_[j][firstShip].type == shipType)
    {
      qDebug() << "found ship type in grid";
      shipIndex = j;
      break;
    }
    else if (rectItems_[j][0].type.isEmpty() && rectItems_[j][maxShips - 1].type.isEmpty())
    {
      shipIndex = j;
      qDebug() << "found empty type in grid";
    }
  }
  qDebug() << "ship type not in grid yet";
  auto& row = rectItems_[shipIndex];
  for (int i = 0; i < maxShips; ++i)
  {
    auto newShipColumn = leftSide ? i : maxShips - i - 1;
    if (row[newShipColumn].type.isEmpty())
    {
      row[newShipColumn].type = shipType;
      row[newShipColumn].isAttacker = isAttacker;
      row[newShipColumn].initiative = initiative;
      row[newShipColumn].item->setBrush(color);
      break;
    }
  }
}

void ShipGraphicsManager::removeShipRect(const QString& name)
{
  qDebug() << __FUNCTION__ << name;
}

void ShipGraphicsManager::reorderShips(const QString& name, int newInitiative)
{
  qDebug() << __FUNCTION__ << name << newInitiative;
}

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
  //qDebug() << "spec accepted:" << name_;
  //std::cout << '\t' << spec_ << std::endl;
  updateSpecLabel();
  if (oldInitiative != spec_.initiative && widgets_.count->intValue() > 0)
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

ShipSpecEditorDialog::ShipSpecEditorDialog(const QString& name, QWidget* parent) : QDialog(parent)
{
  setupUi(this);
  editShipGroupBox_->setTitle(editShipGroupBox_->title() + name);
  setModal(true);
  connect(initiativeSpinBox_, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
    this, &ShipSpecEditorDialog::editSpec);
  connect(hullSpinBox_, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
    this, &ShipSpecEditorDialog::editSpec);
  connect(shieldSpinBox_, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
    this, &ShipSpecEditorDialog::editSpec);
  connect(computerSpinBox_, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
    this, &ShipSpecEditorDialog::editSpec);
  connect(yellowGunSpinBox_, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
    this, &ShipSpecEditorDialog::editSpec);
  connect(orangeGunSpinBox_, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
    this, &ShipSpecEditorDialog::editSpec);
  connect(redGunSpinBox_, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
    this, &ShipSpecEditorDialog::editSpec);
}

void ShipSpecEditorDialog::setSpec(const ShipSpec& spec)
{
  displayedSpec_ = spec;
  initiativeSpinBox_->setValue(spec.initiative);
  hullSpinBox_->setValue(spec.hull);
  shieldSpinBox_->setValue(spec.shield);
  computerSpinBox_->setValue(spec.computer);
  yellowGunSpinBox_->setValue(spec.yellowGuns);
  orangeGunSpinBox_->setValue(spec.orangeGuns);
  redGunSpinBox_->setValue(spec.redGuns);
}

void ShipSpecEditorDialog::editSpec(int newValue)
{
  auto name = sender()->objectName();
  if (name.startsWith("initiative"))
    displayedSpec_.initiative = newValue;
  else if (name.startsWith("hull"))
    displayedSpec_.hull = newValue;
  else if (name.startsWith("shield"))
    displayedSpec_.shield = newValue;
  else if (name.startsWith("computer"))
    displayedSpec_.computer = newValue;
  else if (name.startsWith("yellowGun"))
    displayedSpec_.yellowGuns = newValue;
  else if (name.startsWith("orangeGun"))
    displayedSpec_.orangeGuns = newValue;
  else if (name.startsWith("redGun"))
    displayedSpec_.redGuns = newValue;
}
