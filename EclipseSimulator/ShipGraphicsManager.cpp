#include <ShipGraphicsManager.hpp>

namespace
{
  const int w = 20;
  const int h = 30;
  const int spacing = 10;
  const int maxShipTypes = 7;
  const int border = 5;
}

ShipGraphicsManager::ShipGraphicsManager(QGraphicsScene* scene, QWidget* parent)
  : QObject(parent), scene_(scene)
{}

Qt::BrushStyle getPattern(const QString& ship)
{
  if (ship == "Interceptor")
    return Qt::SolidPattern;
  if (ship == "Cruiser")
    return Qt::Dense1Pattern;
  if (ship == "Dreadnought")
    return Qt::DiagCrossPattern;
  if (ship == "Starbase")
    return Qt::BDiagPattern;
  return Qt::VerPattern;
}

void ShipGraphicsManager::addShipRect(const QString& name, int initiative)
{
  auto desc = name.split(' ');
  auto isAttacker = desc[0] == "Attacker";
  auto shipType = desc[1];
  auto color = isAttacker ? Qt::red : Qt::blue;
  auto pattern = getPattern(shipType);
  auto leftSide = isAttacker;

  auto& row = rectItems_[name];
  for (int i = 0; i < row.size(); ++i)
  {
    if (row[i].type.isEmpty())
    {
      row[i].type = shipType;
      row[i].isAttacker = isAttacker;
      row[i].initiative = initiative;
      row[i].item->setBrush(QBrush(color, pattern));
      row[i].item->setOpacity(1);
      break;
    }
  }
}

void ShipGraphicsManager::removeShipRect(const QString& name)
{
  auto desc = name.split(' ');
  auto isAttacker = desc[0] == "Attacker";
  auto leftSide = isAttacker;

  auto& row = rectItems_[name];
  for (int i = row.size() - 1; i >= 0; --i)
  {
    if (!row[i].type.isEmpty())
    {
      row[i].type = "";
      row[i].item->setBrush(Qt::black);
      row[i].item->setOpacity(0.1);
      if (i == 0)
        reorderShips();
      break;
    }
  }
}

void ShipGraphicsManager::adjustInitiative(const QString& name, int newInitiative)
{
  if (!name.isEmpty())
  {
    auto desc = name.split(' ');
    auto isAttacker = desc[0] == "Attacker";
    auto shipType = desc[1];

    auto& row = rectItems_[name];
    for (int i = 0; i < row.size(); ++i)
    {
      auto& item = row[i];
      if (item.type == shipType && item.isAttacker == isAttacker)
        item.initiative = newInitiative;
    }

    for (auto& desc : descriptionRects_)
    {
      if (desc.type == shipType && desc.isAttacker == isAttacker)
        desc.initiative = newInitiative;
    }
  }
  reorderShips();
}

void ShipGraphicsManager::reorderShips()
{
  std::sort(descriptionRects_.begin(), descriptionRects_.end());
  auto i = 0;
  for (auto& desc : descriptionRects_)
  {
    int newY = i*40 + 4;
    auto oldY = desc.item->pos().y();
    desc.item->moveBy(0, newY - oldY);
    auto& row = rectItems_[desc.name];
    for (int j = 0; j < row.size(); ++j)
    {
      auto oldY = row[j].item->pos().y();
      row[j].item->moveBy(0, newY - oldY);
    }
    ++i;
  }
}

void ShipGraphicsManager::addShipBorders(const std::map<QString, int>& maxShips)
{
  QPen outlinePen(Qt::white);
  outlinePen.setWidth(2);

  auto i = 0;
  for (const auto& name : names_)
  {
    auto desc = name.split(' ');
    auto isAttacker = desc[0] == "Attacker";
    auto leftSide = isAttacker;

    auto maxIter = maxShips.find(name);
    if (maxIter == maxShips.end())
    {
      qDebug() << "Bad ship name:" << name;
      return;
    }
    auto max = maxIter->second;
    rectItems_[name].resize(max);
    for (int j = 0; j < max; ++j)
    {
      auto rectangle = scene_->addRect(0, 0, w, h, outlinePen, Qt::black);
      auto horizontal = leftSide ? j : (7 - j);
      rectangle->setPos(border + horizontal*(w + spacing), border + i*(h + spacing));
      rectItems_[name][j].item = rectangle;
      rectItems_[name][j].name = name;
      rectangle->setOpacity(0.1);
    }
    ++i;
  }
}

void ShipGraphicsManager::addShipDescriptions(const std::vector<QString>& names)
{
  names_ = names;
  QPen outlinePen(Qt::yellow);
  outlinePen.setWidth(4);

  auto i = 0;
  for (const auto& name : names)
  {
    auto desc = name.split(' ');
    auto isAttacker = desc[0] == "Attacker";
    auto shipType = desc[1];
    auto color = isAttacker ? Qt::red : Qt::blue;
    auto pattern = getPattern(shipType);
    auto leftSide = isAttacker;

    auto rectangle = scene_->addRect(0, 0, w, h, outlinePen, QBrush(color, pattern));
    rectangle->setPos(leftSide ? -30 : 250, 5 + 40*i);

    ShipRect r(rectangle, name, shipType, isAttacker, 0);

    descriptionRects_.push_back(r);
    i++;
  }
}
