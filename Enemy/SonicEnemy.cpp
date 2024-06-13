#include "SonicEnemy.h"
SonicEnemy::SonicEnemy(int x, int y, Team team) : Enemy("play/enemy-9.png", x, y,team, 10, 50, 5, 5, 2) {
}
void SonicEnemy::Update(float deltaTime){
    Enemy::Update(deltaTime);
    if(hp<5)speed=200;
}