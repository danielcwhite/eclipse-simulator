#include <GuiShip.hpp>
#include <QDebug>
#include <ShipGraphicsManager.hpp>

GuiShipImpl::GuiShipImpl(ShipGraphicsManager* sgm) : sgm_(sgm) {}

void GuiShipImpl::setActive(bool active, const QString& name, int index, const QString& desc)
{
  sgm_->setShipToActive(active, name, index, desc);
}

void GuiShipImpl::applyDamage(int amount, const QString& name, int index)
{
  sgm_->applyDamage(amount, name, index);
}

void GuiShipImpl::setMatch(const QString& fromName, int fromIndex, const QString& toName, int toIndex)
{
  sgm_->setMatch(fromName, fromIndex, toName, toIndex);
}

void GuiShipImpl::setAsDead(const QString& name, int index)
{
  sgm_->setAsDead(name, index);
}

void GuiShip::setGuiImpl(std::shared_ptr<GuiShipImpl> gsi)
{
  guiImpl_ = gsi;
}

std::string GuiShip::name() const
{
  return name_;
}

int GuiShip::index() const
{
  return index_;
}

bool GuiShip::isAttacker() const
{
  return impl_.isAttacker();
}

bool GuiShip::isFighting(const ShipInterface& other) const
{
  const auto& otherShip = static_cast<const GuiShip&>(other);
  const auto& otherImpl = otherShip.impl_;
  auto match = impl_.isFighting(otherImpl);
  if (match && guiImpl_)
    guiImpl_->setMatch(qName(), index(), otherShip.qName(), otherShip.index());
  return match;
}

bool GuiShip::isAlive() const
{
  return impl_.isAlive();
}

void GuiShip::setAsDead()
{
  if (!isAlive() && guiImpl_)
    guiImpl_->setAsDead(qName(), index());
}

void GuiShip::applyDamage(int amount)
{
  impl_.applyDamage(amount);
  if (guiImpl_)
    guiImpl_->applyDamage(amount, qName(), index());
}

ShipSpec GuiShip::spec() const
{
  return impl_.spec();
}

std::string GuiShip::describe() const
{
  std::ostringstream ostr;
  ostr << impl_;
  return ostr.str();
}

bool GuiShip::lessThan(const ShipInterface& rhs) const
{
 return impl_ < static_cast<const GuiShip&>(rhs).impl_;
}

void GuiShip::setActive(bool active)
{
  if (guiImpl_)
    guiImpl_->setActive(active, QString::fromStdString(name()), index(), qDescribe());
}
