#ifndef TURRET_HPP
#define TURRET_HPP
#include <allegro5/base.h>
#include <list>
#include <string>

#include "Engine/Sprite.hpp"
#include "Engine/Unit.hpp"

class Enemy;
class PlayScene;

class Turret: public Unit {
protected:
    int price;
    int point;
    float coolDown;
    float reload = 0;
    float rotateRadian = 2 * ALLEGRO_PI;
    float range;
    Sprite imgBase;
    std::list<Turret*>::iterator lockedTurretIterator;
    PlayScene* getPlayScene();
    // Reference: Design Patterns - Factory Method.
    std::vector<StatusEffect> effectOnEnemy;
public:
    bool Enabled = true;
    bool Preview = false;
    Enemy* Target = nullptr;
    Turret(std::string imgBase, std::string imgTurret, float x, float y,Team team, float radius, int price, float coolDown, int point);
    void Update(float deltaTime) override;
    void Draw(float scale=1, float cx=0, float cy=0, float sx=0, float sy=0) const override;
	int GetPrice() const;
};
#endif // TURRET_HPP
