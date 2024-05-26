#include <allegro5/base.h>
#include <random>
#include <string>

#include	<iostream>

#include 	"Engine/Point.hpp"
#include	"Enemy/SoldierEnemy.hpp"
#include	"Enemy/TankEnemy.hpp"
#include 	"TruckEnemy.hpp"

TruckEnemy::TruckEnemy(int x, int y) : Enemy("play/enemy-4.png", x, y, 20, 20, 150, 75, 8) {}

void TruckEnemy::UponDeath(){
	Enemy* enemy;
	for(int i=0;i<4;i++){
		getPlayScene()->SpawnEnemy(1,Position.x + Velocity.x * i,Position.y + Velocity.y * i,0.1f*(float)i);
	}
}

