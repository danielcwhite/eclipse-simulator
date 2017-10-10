#ifndef SHIP_SPEC
#define SHIP_SPEC

#include <iostream>

struct ShipSpec
{
  ShipSpec() : hull(0), shield(0), computer(0), yellowGuns(0), orangeGuns(0), redGuns(0), initiative(0) {}
  ShipSpec(int h, int s, int c, int y, int o, int r, int i) :
    hull(h), shield(s), computer(c), yellowGuns(y), orangeGuns(o), redGuns(r), initiative(i) {}
  int hull;
  int shield;
  int computer;
  int yellowGuns;
  int orangeGuns;
  int redGuns;
  int initiative;
};

inline std::ostream& operator<<(std::ostream& o, const ShipSpec& ship)
{
  return o << "ShipSpec(h" << ship.hull
    << ",s" << ship.shield
    << ",c" << ship.computer
    << "," << ship.yellowGuns
    << "d1," << ship.orangeGuns
    << "d2," << ship.redGuns
    << "d4,i" << ship.initiative << ")";
}

#endif
