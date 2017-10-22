#ifndef INTERFACE_APPLICATION_SHIPSPECEDITORDIALOG_H
#define INTERFACE_APPLICATION_SHIPSPECEDITORDIALOG_H

#include "ui_ShipSpecEditor.h"
#include <ShipSpec.hpp>
#include <QDialog>

class ShipSpecEditorDialog : public QDialog, public Ui::ShipSpecEditor
{
  Q_OBJECT
public:
  explicit ShipSpecEditorDialog(const QString& name, QWidget* parent = nullptr);
  ShipSpec spec() const { return displayedSpec_; }
  void setSpec(const ShipSpec& spec);
public Q_SLOTS:
  void editSpec(int newValue);
private:
  ShipSpec displayedSpec_;
};

#endif
