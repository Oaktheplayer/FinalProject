#ifndef TANKENEMY_HPP
#define TANKENEMY_HPP
#include "Enemy.hpp"
#include "Engine/Sprite.hpp"

class TankEnemy : public Enemy {
private:
	Sprite head;
	float targetRotation;
public:
	TankEnemy(int x, int y);
	void Draw(float scale=1, float cx=0, float cy=0, float sx=0, float sy=0) const override;
	void Update(float deltaTime) override;
};
#endif // TANKENEMY_HPP
