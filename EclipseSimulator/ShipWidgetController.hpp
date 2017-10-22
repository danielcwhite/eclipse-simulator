#ifndef INTERFACE_APPLICATION_SHIPWIDGETCONTROLLER_H
#define INTERFACE_APPLICATION_SHIPWIDGETCONTROLLER_H

#include <QObject>

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

#endif
