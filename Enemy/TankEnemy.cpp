#include <allegro5/base.h>
#include <random>
#include <string>
#include "Engine/Point.hpp"
#include "TankEnemy.hpp"
#include "Engine/AudioHelper.hpp"
#include "Bullet/FireBullet.hpp"
#include <iostream>

const int TankEnemy::Price = 25;
TankEnemy::TankEnemy(int x, int y,Team team) : Enemy("play/enemy-3.png", x, y,team, 20, 20, 100, 50, 5,Price),
	head("play/enemy-3-head.png", this,0,0,range,1), targetRotation(0) {
	coolDown	=	1;
	range		=	300;
}
void TankEnemy::Draw(float scale, float cx, float cy, float sx, float sy) const {
	Enemy::Draw(scale, cx, cy, sx, sy);
	head.Draw(scale, cx, cy, sx, sy);
}
void TankEnemy::Update(float deltaTime) {
	head.Position = Position;
	if(!Enabled) return;
	ShootTarget(deltaTime);
	if(!Target) head.Rotation	=	Rotation;
    Enemy::Update(deltaTime);
}

void TankEnemy::CreateBullet() {
	Engine::Point diff = Engine::Point(cos(head.Rotation), sin(head.Rotation));
	float rotation = atan2(diff.y, diff.x);
	Engine::Point normalized = diff.Normalize();
	// Change bullet position to the front of the gun barrel.
	getPlayScene()->BulletGroup->AddNewObject(new FireBullet(Position + normalized * 36, diff, rotation, this));
	AudioHelper::PlayAudio("gun.wav");
}
void TankEnemy::Kill()
{
	Enemy::Kill();
}
// TODO: Head rotation error
void TankEnemy::RotateHead(float deltaTime){
	head.RotateTurret(deltaTime);
    //std::cerr<<"rotate head\n";
}