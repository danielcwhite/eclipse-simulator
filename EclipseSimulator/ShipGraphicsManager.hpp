#ifndef INTERFACE_APPLICATION_SHIPGRAPHICSMANAGER_H
#define INTERFACE_APPLICATION_SHIPGRAPHICSMANAGER_H

#include <QObject>
#include <QGraphicsItem>
#include <QtWidgets>

class ShipGraphicsManager : public QObject
{
  Q_OBJECT
public:
  explicit ShipGraphicsManager(QGraphicsScene* scene, QWidget* parent = nullptr);
  void addShipBorders(const std::map<QString, int>& maxShips);
  void addShipDescriptions(const std::vector<QString>& names);
public Q_SLOTS:
  void addShipRect(const QString& name, int initiative);
  void removeShipRect(const QString& name);
  void adjustInitiative(const QString& name, int newInitiative);
  void reorderShips();
  void setShipToActive(bool active, const QString& name, int index, const QString& desc);
  void applyDamage(int amount, const QString& name, int index);
private:
  QGraphicsScene* scene_;
  struct ShipRect
  {
    ShipRect() {}
    ShipRect(QGraphicsRectItem* i, const QString& n, const QString& t, bool a, int o) :
      item(i), name(n), type(t), isAttacker(a), initiative(o) {}

    QGraphicsRectItem* item;
    QString name, type;
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

  std::vector<QString> names_;
  std::map<QString, std::vector<ShipRect>> rectItems_;
  std::vector<ShipRect> descriptionRects_;
};

#endif
