#ifndef INTERFACE_APPLICATION_SCIRUN_MAIN_WINDOW_H
#define INTERFACE_APPLICATION_SCIRUN_MAIN_WINDOW_H

#include <QMainWindow>
#include <ShipSpec.hpp>
#include <sstream>
#include "ui_EclipseSimulatorMainWindow.h"
#include "ui_ShipSpecEditor.h"

class ShipWidgetController;
class ShipGraphicsManager;

class EclipseMainWindow : public QMainWindow, public Ui::EclipseSimMainWindow
{
  Q_OBJECT

public:
  EclipseMainWindow();

private Q_SLOTS:
  void startBattle();
  void incrementBattle();
  void finishBattle();

private:
  void setupBattleOrderView();
  void log(const QString& str);

  QGraphicsScene* scene_;
  std::vector<ShipWidgetController*> ships_;
  ShipGraphicsManager* shipGraphics_;
};

template <class T>
QString toString(const T& t)
{
  std::ostringstream o;
  o << t;
  return QString::fromStdString(o.str());
}

#endif
