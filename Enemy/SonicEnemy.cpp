#include "SonicEnemy.h"
const int SonicEnemy::Price = 30;
SonicEnemy::SonicEnemy(int x, int y, Team team) : Enemy("play/jeep_troop.png", x, y,team, 10, 50, 30, 5, 2,Price) {
}
void SonicEnemy::Update(float deltaTime){
    Enemy::Update(deltaTime);
    if(hp<15)speed=200;
}