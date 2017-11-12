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

void ShipSpecEditorDialog::presetSelected(const QString& name)
{
  if (name == "Base Terran Interceptor")
  {
    setSpec({0, 0, 0, 1, 0, 0, 2});
  }
  else if (name == "Base Terran Cruiser")
  {
    qDebug() << "TODO";
  }
  else if (name == "Base Terran Dreadnought")
  {
    qDebug() << "TODO";
  }
  else if (name == "Base Terran Starbase")
  {
    qDebug() << "TODO";
  }
  else if (name == "Ancient Interceptor")
  {
    qDebug() << "TODO";
  }
  else if (name == "Galactic Civil Defense System")
  {
    qDebug() << "TODO";
  }
  else if (name == "Ancient Cruiser #1")
  {
    qDebug() << "TODO";
  }
  else if (name == "Ancient Cruiser #2")
  {
    qDebug() << "TODO";
  }
  else if (name == "Ancient Cruiser #3")
  {
    qDebug() << "TODO";
  }
  else if (name == "Ancient Cruiser #4")
  {
    qDebug() << "TODO";
  }
  else if (name == "Ancient Dreadnought #1")
  {
    qDebug() << "TODO";
  }
  else if (name == "Ancient Dreadnought #2")
  {
    qDebug() << "TODO";
  }

}
