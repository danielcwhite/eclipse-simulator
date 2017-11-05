#include <GuiShip.hpp>
#include <QDebug>
#include <ShipGraphicsManager.hpp>

GuiShipImpl::GuiShipImpl(ShipGraphicsManager* sgm) : sgm_(sgm) {}

void GuiShipImpl::setActive(bool active, const QString& name, int index, const QString& desc)
{
  sgm_->setShipToActive(active, name, index, desc);
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
  return impl_.isFighting(static_cast<const GuiShip&>(other).impl_);
}

bool GuiShip::isAlive() const
{
  return impl_.isAlive();
}

void GuiShip::applyDamage(int amount)
{
  impl_.applyDamage(amount);
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
