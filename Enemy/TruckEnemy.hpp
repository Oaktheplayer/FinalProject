#ifndef TRUCKENEMY_HPP
#define TRUCKENEMY_HPP
#include "Enemy.hpp"

class TruckEnemy : public Enemy {
public:
	TruckEnemy(int x, int y);
	void UponDeath() override;
};
#endif // TRUCKENEMY_HPP
