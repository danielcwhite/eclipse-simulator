#ifndef INTERFACE_APPLICATION_SCIRUN_MAIN_WINDOW_H
#define INTERFACE_APPLICATION_SCIRUN_MAIN_WINDOW_H

#include <QMainWindow>
#include <ShipSpec.hpp>
#include "ui_EclipseSimulatorMainWindow.h"
#include "ui_ShipSpecEditor.h"

struct ShipWidgets
{
  QLabel* description;
  QPushButton* edit;
  QToolButton* add;
  QToolButton* remove;
  QLCDNumber* count;
};

class ShipWidgetController : public QObject
{
  Q_OBJECT
public:
  ShipWidgetController(ShipWidgets widgets, const QString& name, int maxShips, QWidget* parent = nullptr);
public Q_SLOTS:
  void addShipPressed();
  void removeShipPressed();
  void editShipPressed();
  void specAccepted();
  void specRejected();
private:
  void updateSpecLabel();
  ShipWidgets widgets_;
  QString name_;
  const int maxShips_;
  class ShipSpecEditorDialog* editor_;
  ShipSpec spec_;
};

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

class EclipseMainWindow : public QMainWindow, public Ui::EclipseSimMainWindow
{
  Q_OBJECT

public:
  EclipseMainWindow();

private:
  void setupBattleOrderView();

  QGraphicsScene* scene_;
  std::vector<ShipWidgetController*> ships_;
};


#endif
