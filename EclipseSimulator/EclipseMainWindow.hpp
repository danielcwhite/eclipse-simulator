#ifndef INTERFACE_APPLICATION_SCIRUN_MAIN_WINDOW_H
#define INTERFACE_APPLICATION_SCIRUN_MAIN_WINDOW_H

#include <QMainWindow>
#include <ShipSpec.hpp>
#include <sstream>
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
  QString name() const { return name_; }

  const ShipSpec& spec() const { return spec_; }
  bool isAttacker() const { return name().split(' ')[0] == "Attacker"; }
  int activeCount() const { return widgets_.count->value(); }
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
  void addShipDescriptions(const std::vector<QString>& names);
public Q_SLOTS:
  void addShipRect(const QString& name, int initiative);
  void removeShipRect(const QString& name);
  void adjustInitiative(const QString& name, int newInitiative);
  void reorderShips();
private:
  QGraphicsScene* scene_;
  struct ShipRect
  {
    ShipRect() {}
    ShipRect(QGraphicsRectItem* i, const QString& t, bool a, int o) :
      item(i), type(t), isAttacker(a), initiative(o) {}

    QGraphicsRectItem* item;
    QString type;
    bool isAttacker {false};
    int initiative {0};

    bool operator<(const ShipRect& other) const
    {
      if (initiative > other.initiative)
        return true;
      if (initiative == other.initiative)
        return isAttacker < other.isAttacker;
      return false;
    }
  };
  std::map<int, std::map<int, ShipRect>> rectItems_;
  std::vector<ShipRect> descriptionRects_;
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

  struct Appender
  {
    EclipseMainWindow* window;
  };
  friend class Appender;
  void append(const QString& str);

private Q_SLOTS:
  void startBattle();
  void incrementBattle();
  void finishBattle();

private:
  void setupBattleOrderView();
  void log(const QString& str);

  Appender& getAppender() { return appender; }

  QGraphicsScene* scene_;
  std::vector<ShipWidgetController*> ships_;
  ShipGraphicsManager* shipGraphics_;
  Appender appender;
};

template <class T>
QString toString(const T& t)
{
  std::ostringstream o;
  o << t;
  return QString::fromStdString(o.str());
}

template <class T>
EclipseMainWindow::Appender& operator<<(EclipseMainWindow::Appender& a, const T& t)
{
  a.window->append(toString(t));
  return a;
}

#endif
