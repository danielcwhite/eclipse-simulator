
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
