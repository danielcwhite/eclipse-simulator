#ifndef INTERFACE_APPLICATION_SHIPGRAPHICSMANAGER_H
#define INTERFACE_APPLICATION_SHIPGRAPHICSMANAGER_H

#include <QObject>
#include <QGraphicsItem>
#include <QtWidgets>

class ShipGraphicsItem
{
public:
  ShipGraphicsItem() {}
  ShipGraphicsItem(QGraphicsRectItem* i, const QString& n, const QString& t, bool a, int o) :
    item(i), name(n), type(t), isAttacker(a), initiative(o) {}

  QGraphicsRectItem* item;
  QString name, type;
  bool isAttacker {false};
  int initiative {0};
  int hits {0};

  bool operator<(const ShipGraphicsItem& other) const
  {
    if (initiative > other.initiative)
      return true;
    if (initiative == other.initiative)
      return isAttacker < other.isAttacker;
    return false;
  }

  void applyDamage(int amount);
  void clearDamage();
  void setHitpoints(int hitpoints);
  void clearHitpoints();
private:
  std::vector<QGraphicsRectItem*> damageCubes_;
  QGraphicsSimpleTextItem* hitpoints_;
};

class ShipGraphicsManager : public QObject
{
  Q_OBJECT
public:
  explicit ShipGraphicsManager(QGraphicsScene* scene, QWidget* parent = nullptr);
  void addShipBorders(const std::map<QString, int>& maxShips);
  void addShipDescriptions(const std::vector<QString>& names);
  void setShipHitpoints(const std::map<QString, int>& shipHitpoints);
public Q_SLOTS:
  void addShipRect(const QString& name, int initiative);
  void removeShipRect(const QString& name);
  void adjustInitiative(const QString& name, int newInitiative);
  void reorderShips();
  void setShipToActive(bool active, const QString& name, int index, const QString& desc);
  void applyDamage(int amount, const QString& name, int index);
  void clearDamage();
private:
  QGraphicsScene* scene_;

  std::vector<QString> names_;
  std::map<QString, std::vector<ShipGraphicsItem>> rectItems_;
  std::vector<ShipGraphicsItem> descriptionRects_;
};

#endif
