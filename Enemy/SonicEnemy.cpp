#include "SonicEnemy.h"
const int SonicEnemy::Price = 50;
SonicEnemy::SonicEnemy(int x, int y, Team team) : Enemy("play/jeep_troop.png", x, y,team, 10, 50, 5, 5, 2,Price) {
}
void SonicEnemy::Update(float deltaTime){
    Enemy::Update(deltaTime);
    if(hp<5)speed=200;
}