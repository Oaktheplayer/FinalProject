#ifndef PLANEENEMY_HPP
#define PLANEENEMY_HPP
#include "Enemy.hpp"
#include    <map>

class PlaneEnemy : public Enemy {
private:
    Engine::Point targetPos;
    float rotateRadian = 2 * ALLEGRO_PI;
public:
    static const int Price;
	PlaneEnemy(int x, int y,Team team);
    void AirTroopTargetFinding() override;
    bool AirTroopUpdate(float deltaTime) override;
    bool toEnd;
    float escapeCoolDown;
    float escapeReload = 0;
    virtual void CreateBullet() override;
};
#endif // PLANEENEMY_HPP
