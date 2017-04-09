#include <iostream>

struct Ship
{
  int hull;
  int shield;
  int computer;
  int yellowGuns;
  int orangeGuns;
  int redGuns;
  int initiative;
};

Ship readShip()
{
  std::cout << "Enter ship details:\n";
  std::cout << "hull: ";
  int hull;
  std::cin >> hull;

  return Ship { hull, 0,0,1,0,0,1 };
}


int main(int argc, char** argv)
{
  std::cout << "Eclipse man!" << std::endl;

  auto s1 = readShip();

  std::cout << "Read this ship:\n" 
    << s1.hull << std::endl;

  return 0;
}
