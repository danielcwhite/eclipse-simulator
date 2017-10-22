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
  std::map<QString, std::map<int, ShipRect>> rectItems_;
  std::vector<ShipRect> descriptionRects_;
};

#endif
