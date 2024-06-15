#ifndef SOLDIERENEMY_HPP
#define SOLDIERENEMY_HPP
#include "Enemy.hpp"

class SoldierEnemy : public Enemy {
public:
    static const int Price;
	SoldierEnemy(int x, int y,Team team);
};
#endif // SOLDIERENEMY_HPP
