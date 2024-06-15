#ifndef PLANEENEMY_HPP
#define PLANEENEMY_HPP
#include "Enemy.hpp"

class PlaneEnemy : public Enemy {
public:
    static const int Price;
	PlaneEnemy(int x, int y,Team team);
};
#endif // PLANEENEMY_HPP
