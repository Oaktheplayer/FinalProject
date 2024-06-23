#ifndef ACV_HPP
#define ACV_HPP
#include "Enemy.hpp"
#include "Engine/Sprite.hpp"
#include "TroopTurret/TroopTurret.hpp"

class ACV : public Enemy {
private:
	TroopTurret head;
	Point	turretOffset;
	float targetRotation;
	float reload;
public:
    static const int Price;
    static const int TurretRange;
	ACV(int x, int y,Team team);
	void Draw(float scale=1, float cx=0, float cy=0, float sx=0, float sy=0) const override;
	void Update(float deltaTime) override;
	void RotateHead(float deltaTime) override;
	void CreateBullet() override;
	void Kill() override;
};
#endif // ACV_HPP
