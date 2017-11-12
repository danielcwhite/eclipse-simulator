#include <ShipSpecEditorDialog.hpp>
#include <QDebug>

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

  presetConfigComboBox_->addItems(presetSpecs_.keys());
  connect(presetConfigComboBox_, SIGNAL(activated(const QString&)), this, SLOT(presetSelected(const QString&)));
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

//hull(0), shield(0), computer(0), yellowGuns(0), orangeGuns(0), redGuns(0), initiative(0)
void ShipSpecEditorDialog::presetSelected(const QString& name)
{
  auto iter = presetSpecs_.find(name);
  if (iter != presetSpecs_.end())
  {
    setSpec(iter.value());
  }
}

QMap<QString, ShipSpec> ShipSpecEditorDialog::presetSpecs_ =
{
  { "Terran Interceptor", {0, 0, 0, 1, 0, 0, 3} },
  { "Terran Cruiser", {1, 0, 1, 1, 0, 0, 2} },
  { "Terran Dreadnought", {2, 0, 1, 2, 0, 0, 1} },
  { "Terran Starbase", {2, 0, 1, 1, 0, 0, 4} },
  { "Ancient Interceptor", {1, 0, 1, 2, 0, 0, 2} },
  { "Galactic Civil Defense System", {7, 0, 1, 4, 0, 0, 0} },
  { "Ancient Cruiser #1", {2, 0, 2, 3, 0, 0, 4} },
  { "Ancient Cruiser #2--INCOMPLETE", {1, 0, 2, 2, 0, 0, 1} },  //TODO: +2 orange missiles
  { "Ancient Cruiser #3", {2, 1, 1, 4, 0, 0, 0} },
  { "Ancient Cruiser #4--INCOMPLETE", {4, 0, 2, 2, 0, 0, 2} },  //TODO special ability: point defense
  { "Ancient Cruiser #5--INCOMPLETE", {1, 0, 2, 0, 1, 0, 2} },  //TODO: +1 red missile //TODO special ability: +2 distortion shield
  { "Ancient Cruiser #6", {2, 0, 1, 0, 2, 0, 3} },
  { "Ancient Cruiser #7", {4, 0, 3, 0, 0, 1, 0} },
  { "Ancient Cruiser #8", {2, 3, 1, 3, 0, 0, 0} },
  { "Ancient Dreadnought #1--INCOMPLETE", {5, 0, 2, 0, 2, 0, 3} }, // TODO: special ability, regeneration: remove one damage cube per round
  { "Ancient Dreadnought #2--INCOMPLETE", {3, 0, 2, 0, 0, 1, 4} }, // TODO: +4 yellow missiles
  { "Rho Indi Syndicate Interceptor", {0, 1, 0, 1, 0, 0, 4} },
  { "Rho Indi Syndicate Cruiser", {1, 1, 1, 1, 0, 0, 3} },
  { "Rho Indi Syndicate Starbase", {2, 1, 1, 1, 0, 0, 4} },
  { "Orion Hegemony Interceptor", {0, 1, 0, 1, 0, 0, 4} },
  { "Orion Hegemony Cruiser", {1, 1, 1, 1, 0, 0, 3} },
  { "Orion Hegemony Dreadnought", {2, 1, 1, 2, 0, 0, 2} },
  { "Orion Hegemony Starbase", {2, 1, 1, 1, 0, 0, 5} },
  { "Planta Interceptor", {0, 0, 1, 1, 0, 0, 1} },
  { "Planta Cruiser", {1, 0, 1, 1, 0, 0, 1} },
  { "Planta Dreadnought", {2, 0, 1, 2, 0, 0, 1} },
  { "Planta Starbase", {2, 0, 1, 1, 0, 0, 2} },
  { "Exiles Orbital", {3, 0, 0, 2, 0, 0, 0} }
};
//hull(0), shield(0), computer(0), yellowGuns(0), orangeGuns(0), redGuns(0), initiative(0)
