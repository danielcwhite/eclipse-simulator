#include <ShipGraphicsManager.hpp>

namespace
{
  const int w = 20;
  const int h = 30;
  const int spacing = 10;
  const int maxShipTypes = 7;
  const int maxShips = 8;
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

  // int firstShip = leftSide ? 0 : maxShips - 1;
  // int shipIndex = -1;
  // for (int j = 0; j < maxShipTypes; ++j)
  // {
  //   if (descriptionRects_[j].type == shipType && descriptionRects_[j].isAttacker == isAttacker)
  //   {
  //     shipIndex = j;
  //     break;
  //   }
  // }

  auto& row = rectItems_[name];
  for (int i = 0; i < maxShips; ++i)
  {
    auto newShipColumn = leftSide ? i : maxShips - i - 1;
    if (row[newShipColumn].type.isEmpty())
    {
      row[newShipColumn].type = shipType;
      row[newShipColumn].isAttacker = isAttacker;
      row[newShipColumn].initiative = initiative;
      row[newShipColumn].item->setBrush(QBrush(color, pattern));
      row[newShipColumn].item->setOpacity(1);
      break;
    }
  }
}

void ShipGraphicsManager::removeShipRect(const QString& name)
{
  auto desc = name.split(' ');
  auto isAttacker = desc[0] == "Attacker";
  // auto shipType = desc[1];
  auto leftSide = isAttacker;
  //
  // int firstShip = leftSide ? 0 : maxShips - 1;
  // int shipIndex = -1;
  // for (int j = 0; j < maxShipTypes; ++j)
  // {
  //   if (descriptionRects_[j].type == shipType && descriptionRects_[j].isAttacker == isAttacker)
  //   {
  //     shipIndex = j;
  //     break;
  //   }
  // }

  auto& row = rectItems_[name];
  for (int i = maxShips - 1; i >= 0; --i)
  {
    auto newShipColumn = leftSide ? i : maxShips - i - 1;
    if (!row[newShipColumn].type.isEmpty())
    {
      row[newShipColumn].type = "";
      row[newShipColumn].item->setBrush(Qt::black);
      row[newShipColumn].item->setOpacity(0.1);
      // if (i == 0)
      //   reorderShips();
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
    for (const auto& n : names_)
    {
      for (int i = 0; i < maxShips; ++i)
      {
        auto& item = rectItems_[n][i];
        if (item.type == shipType && item.isAttacker == isAttacker)
          item.initiative = newInitiative;
      }
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
    for (int j = 0; j < maxShips; ++j)
    {
      auto oldY = row[j].item->pos().y();
      row[j].item->moveBy(0, newY - oldY);
    }
    ++i;
  }
}

void ShipGraphicsManager::addShipBorders()
{
  QPen outlinePen(Qt::white);
  outlinePen.setWidth(2);

  auto i = 0;
  for (const auto& name : names_)
  {
    rectItems_[name].resize(maxShips);
    for (int j = 0; j < maxShips; ++j)
    {
      auto rectangle = scene_->addRect(0, 0, w, h, outlinePen, Qt::black);
      rectangle->setPos(border + j*(w + spacing), border + i*(h + spacing));
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
  QPen outlinePen(Qt::white);
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

    auto rectangle = scene_->addRect(0, 0, w, h, outlinePen, QBrush(color, pattern));
    rectangle->setPos(leftSide ? -30 : 250, 5 + 40*i);

    ShipRect r(rectangle, name, shipType, isAttacker, 0);

    descriptionRects_.push_back(r);
    i++;
  }
}
