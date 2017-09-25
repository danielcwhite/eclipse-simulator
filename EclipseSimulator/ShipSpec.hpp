#ifndef SHIP_SPEC
#define SHIP_SPEC

#include <iostream>

struct ShipSpec
{
  int hull {0};
  int shield {0};
  int computer {0};
  int yellowGuns {0};
  int orangeGuns {0};
  int redGuns {0};
  int initiative {0};
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
