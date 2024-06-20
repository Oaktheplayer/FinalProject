#ifndef TROOPTURRET_HPP
#define TROOPTURRET_HPP
#include <allegro5/base.h>
#include <list>
#include <string>

#include "Engine/Sprite.hpp"
#include "Engine/Unit.hpp"
#include "Building/Building.hpp"
#include <functional>

class Enemy;
class PlayScene;
class Building;

class TroopTurret: public Sprite{
protected:
    Enemy* Parent;
    Point offset;
    int range;
    float coolDown;
    float reload;
    Unit* Target = nullptr;
    float rotateRadian = 2 * ALLEGRO_PI;
    //std::list<Turret*>::iterator lockedTurretIterator;
    PlayScene* getPlayScene();
    // Reference: Design Patterns - Factory Method.
    std::vector<StatusEffect> effectOnEnemy;
public:
    TroopTurret(std::string img, Enemy* parent, float osx, float osy, float range, float coolDown);
    void Update(float deltaTime) override;
    //void Draw(float scale=1, float cx=0, float cy=0, float sx=0, float sy=0) const override;
    void RotateTurret(float deltaTime);
    virtual void CreateBullet();
};
#endif // TURRET_HPP
