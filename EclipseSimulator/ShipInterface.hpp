#ifndef SHIP_INTERFACE_H__
#define SHIP_INTERFACE_H__

#include "ShipSpec.hpp"

class ShipInterface
{
public:
  virtual ~ShipInterface() {}
  virtual std::string name() const = 0;
  virtual bool isAttacker() const = 0;
  virtual bool isFighting(const ShipInterface& other) const = 0;
  virtual bool isAlive() const = 0;
  virtual void applyDamage(int amount) = 0;
  virtual ShipSpec spec() const = 0;
  virtual std::string describe() const = 0;
  virtual bool lessThan(const ShipInterface& rhs) const = 0;

  virtual void setActive(bool active) = 0;
};

using ShipPtr = std::shared_ptr<ShipInterface>;

#endif
