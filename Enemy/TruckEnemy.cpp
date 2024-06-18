#include <allegro5/base.h>
#include <random>
#include <string>

#include	<iostream>

#include 	"Engine/Point.hpp"
#include	"Enemy/SoldierEnemy.hpp"
#include	"Enemy/TankEnemy.hpp"
#include 	"TruckEnemy.hpp"
const int TruckEnemy::Price = 40;
TruckEnemy::TruckEnemy(int x, int y,Team team) : Enemy("play/truck_troop.png", x, y,team, 20, 20, 75, 35, 8,Price) {}
//TODO: Make it spawn soldier repeatedly
void TruckEnemy::Kill(){
	Enemy* enemy;
	for(int i=0;i<4;i++){
		enemy = getPlayScene()->SpawnEnemy(1,Position.x,Position.y);
		if(!pathBlock)
			enemy->Update(0.5f*i);
	}
	Enemy::Kill();
}

