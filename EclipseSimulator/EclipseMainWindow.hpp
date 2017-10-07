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
Q_SIGNALS:
  void shipAdded(const QString& name, int initiative);
  void shipRemoved(const QString& name);
  void initiativeChanged(const QString& name, int newInitiative);
private:
  void updateSpecLabel();
  ShipWidgets widgets_;
  QString name_;
  const int maxShips_;
  class ShipSpecEditorDialog* editor_;
  ShipSpec spec_;
};

class ShipGraphicsManager : public QObject
{
  Q_OBJECT
public:
  explicit ShipGraphicsManager(QGraphicsScene* scene, QWidget* parent = nullptr);
  void addShipBorders();
public Q_SLOTS:
  void addShipRect(const QString& name, int initiative);
  void removeShipRect(const QString& name);
  void adjustInitiative(const QString& name, int newInitiative);
  void reorderShips();
private:
  QGraphicsScene* scene_;
  struct ShipRect
  {
    QGraphicsRectItem* item;
    QString type;
    bool isAttacker {false};
    int initiative {0};
  };
  std::map<int, std::map<int, ShipRect>> rectItems_;
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
  ShipGraphicsManager* shipGraphics_;
};


#endif
