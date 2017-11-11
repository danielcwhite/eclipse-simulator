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
  QPen outlinePen(Qt::darkGreen);
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
  QPen outlinePen(Qt::darkGreen);
  outlinePen.setWidth(2);

  auto i = 0;
  for (const auto& name : names)
  {
    auto desc = name.split(' ');
    auto isAttacker = desc[0] == "Attacker";
    auto shipType = desc[1];
    auto color = isAttacker ? Qt::red : Qt::blue;
    auto pattern = getPattern(shipType);
    auto leftSide = isAttacker;

    auto rectangle = scene_->addRect(0, 0, 3*w, h, outlinePen, QBrush(color, pattern));
    rectangle->setPos(leftSide ? -30-2*w : 250, 5 + 40*i);

    descriptionRects_.emplace_back(rectangle, name, shipType, isAttacker, 0);
    i++;
  }
}

void ShipGraphicsManager::setShipToActive(bool active, const QString& name, int index, const QString& desc)
{
  QColor color = active ? Qt::white : Qt::darkGreen;
  QPen outlinePen(color);
  outlinePen.setWidth(active ? 4 : 2);
  for (auto& d : descriptionRects_)
  {
    if (d.name == name)
    {
      d.item->setPen(outlinePen);
      break;
    }
  }
  auto iter = rectItems_.find(name);
  if (iter != rectItems_.end())
  {
    iter->second[index].item->setPen(outlinePen);
  }
}

void ShipGraphicsManager::applyDamage(int amount, const QString& name, int index)
{
  auto iter = rectItems_.find(name);
  if (iter != rectItems_.end())
  {
    auto& ship = iter->second[index];
    ship.applyDamage(amount);
  }
}

void ShipGraphicsItem::applyDamage(int amount)
{
  auto rect = item->boundingRect();
  auto pos = item->pos();
  for (int i = 0; i < amount; ++i)
  {
    auto dot = item->scene()->addRect(0, 0, 5, 5, QPen(), QColor(255,0,255));
    dot->setPos(pos + QPoint((hits / 3) * 7 , (hits % 3) * 7));
    damageCubes_.push_back(dot);
    hits++;
  }
}

void ShipGraphicsManager::setAsDead(const QString& name, int index)
{
  auto iter = rectItems_.find(name);
  if (iter != rectItems_.end())
  {
    auto& ship = iter->second[index];
    ship.addDeathLines();
  }
}

void ShipGraphicsItem::addDeathLines()
{
  auto rect = item->boundingRect();
  QPen death(Qt::red);
  death.setWidth(6);
  deathLines_.push_back(item->scene()->addLine(rect.topLeft().x(), rect.topLeft().y(),
    rect.bottomRight().x(), rect.bottomRight().y(), death));
  deathLines_.push_back(item->scene()->addLine(rect.topRight().x(), rect.topRight().y(),
    rect.bottomLeft().x(), rect.bottomLeft().y(), death));
  deathLines_[0]->setPos(item->pos());
  deathLines_[1]->setPos(item->pos());
}

void ShipGraphicsItem::clearDamage()
{
  for (auto& d : damageCubes_)
    delete d;
  damageCubes_.clear();

  for (auto& d : deathLines_)
    delete d;
  deathLines_.clear();
}

void ShipGraphicsManager::clearDamage()
{
  for (auto& row : rectItems_)
  {
    for (auto& ship : row.second)
    {
      if (!ship.type.isEmpty())
        ship.clearDamage();
    }
  }
}

void ShipGraphicsManager::setShipHitpoints(const std::map<QString, int>& shipHitpoints)
{
  for (auto& row : rectItems_)
  {
    for (auto& ship : row.second)
    {
      if (!ship.type.isEmpty())
        ship.clearHitpoints();
    }
  }

  for (const auto& nameHp : shipHitpoints)
  {
    auto iter = rectItems_.find(nameHp.first);
    if (iter != rectItems_.end())
    {
      for (auto& ship : iter->second)
        if (!ship.type.isEmpty())
          ship.setHitpoints(nameHp.second);
    }
  }
}

void ShipGraphicsItem::setHitpoints(int amount)
{
  auto rect = item->boundingRect();
  auto pos = item->pos();
  hitpoints_ = item->scene()->addSimpleText(QString::number(amount));
  hitpoints_->setPos(pos + QPoint(rect.width(), -10));
  hitpoints_->setBrush(QColor(255,182,193)); //pink
}

void ShipGraphicsItem::clearHitpoints()
{
  delete hitpoints_;
  hitpoints_ = nullptr;
}

void ShipGraphicsManager::setMatch(const QString& fromName, int fromIndex, const QString& toName, int toIndex)
{
  return;
  #if 0
  qDebug() << __FUNCTION__ << fromName << fromIndex << toName << toIndex;
  auto& from = rectItems_[fromName][fromIndex];
  auto& to = rectItems_[toName][toIndex];
  auto fromPoint = from.item->pos() + QPoint(20,20);
  auto toPoint = to.item->pos() + QPoint(20,20);

  delete attackLine_;
  QPen atk(Qt::red);
  atk.setWidth(2);
  attackLine_ = scene_->addLine(fromPoint.x(), fromPoint.y(), toPoint.x(), toPoint.y(), atk);
  attackLine_->setPos(fromPoint);
  #endif
}
