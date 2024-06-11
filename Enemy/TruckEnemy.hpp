#ifndef TRUCKENEMY_HPP
#define TRUCKENEMY_HPP
#include "Enemy.hpp"

class TruckEnemy : public Enemy {
public:
	TruckEnemy(int x, int y,Team team);
	void Kill() override;
};
#endif // TRUCKENEMY_HPP
