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
    Sprite imgBase;
    //std::list<Turret*>::iterator lockedTurretIterator;
    PlayScene* getPlayScene();
    // Reference: Design Patterns - Factory Method.
    std::vector<StatusEffect> effectOnEnemy;
public:
    Turret(std::string imgBase, std::string imgTurret, float x, float y,Team team, float radius, int price, float coolDown, int point);
    void Update(float deltaTime) override;
    virtual void Kill() override;
    void Draw(float scale=1, float cx=0, float cy=0, float sx=0, float sy=0) const override;
    int GetHp() const;
    int GetPrice() const override;
};
#endif // TURRET_HPP
