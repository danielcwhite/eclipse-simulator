#ifndef INTERFACE_APPLICATION_SCIRUN_MAIN_WINDOW_H
#define INTERFACE_APPLICATION_SCIRUN_MAIN_WINDOW_H

#include <QMainWindow>
#include "ui_EclipseSimulatorMainWindow.h"

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
  ShipWidgetController(ShipWidgets widgets, const QString& name, int maxShips, QObject* parent = nullptr);
public Q_SLOTS:
  void addShipPressed();
  void removeShipPressed();
  void editShipPressed();
private:
  ShipWidgets widgets_;
  QString name_;
  const int maxShips_;
};

class EclipseMainWindow : public QMainWindow, public Ui::EclipseSimMainWindow
{
  Q_OBJECT

public:
  EclipseMainWindow();

private:
  std::vector<ShipWidgetController*> ships_;
};


#endif
