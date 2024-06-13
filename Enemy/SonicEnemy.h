#ifndef FINALPROJECT_SONICENEMY_H
#define FINALPROJECT_SONICENEMY_H
#include "Enemy.hpp"

class SonicEnemy : public Enemy {
public:
    SonicEnemy(int x, int y,Team team);
    void Update(float deltaTime) override;
};

#endif //FINALPROJECT_SONICENEMY_H
