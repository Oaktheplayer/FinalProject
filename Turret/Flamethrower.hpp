#ifndef FLAMETHROWER_HPP
#define FLAMETHROWER_HPP
#include "Turret.hpp"

class Flamethrower: public Turret {
public:
	static const int Price;
    Flamethrower(float x, float y);
	void CreateBullet() override;
};
#endif // FLAMETHROWER_HPP
