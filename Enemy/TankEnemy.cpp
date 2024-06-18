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
head("play/enemy-3-head.png", x, y), targetRotation(0) {
	coolDown	=	1;
	range		=	300;
}
void TankEnemy::Draw(float scale, float cx, float cy, float sx, float sy) const {
	Enemy::Draw(scale, cx, cy, sx, sy);
	head.Draw(scale, cx, cy, sx, sy);
}
void TankEnemy::Update(float deltaTime) {
	
	if(!Enabled) return;
	Enemy::Update(deltaTime);
	//head.Update(deltaTime);
	head.Position = Position;
	// Choose arbitrary one.
	// std::random_device dev;
	// std::mt19937 rng(dev());
	// std::uniform_real_distribution<> dist(0.0f, 4.0f);
	// float rnd = dist(rng);
	// if (rnd < deltaTime) {
	// 	// Head arbitrary rotation.
	// 	std::uniform_real_distribution<> distRadian(-ALLEGRO_PI, ALLEGRO_PI);
	// 	targetRotation = distRadian(rng);
	// }
	//std::cerr<<(!Target? "No":(std::to_string(Target->Position.x)+","+std::to_string(Target->Position.y)))<<'\n';
	ShootTarget(deltaTime);
	if(!Target) head.Rotation	=	Rotation;
	// head.Rotation = (head.Rotation + deltaTime * targetRotation) / (1 + deltaTime);
}

void TankEnemy::CreateBullet() {
	Engine::Point diff = Engine::Point(cos(head.Rotation), sin(head.Rotation));
	float rotation = atan2(diff.y, diff.x);
	Engine::Point normalized = diff.Normalize();
	// Change bullet position to the front of the gun barrel.
	getPlayScene()->BulletGroup->AddNewObject(new FireBullet(Position + normalized * 36, diff, rotation, this));
	AudioHelper::PlayAudio("gun.wav");
}
//DONE: Head rotation error
void TankEnemy::RotateHead(float deltaTime){
	Engine::Point originRotation = Engine::Point(cos(head.Rotation), sin(head.Rotation));
	Engine::Point targetRotation = (Target->Position - Position).Normalize();
	float maxRotateRadian = rotateRadian * deltaTime;
	float cosTheta = originRotation.Dot(targetRotation);
	// Might have floating-point precision error.
	if (cosTheta > 1) cosTheta = 1;
	else if (cosTheta < -1) cosTheta = -1;
	float radian = acos(cosTheta);
	Engine::Point rotation;
	if (abs(radian) <= maxRotateRadian)
		rotation = targetRotation;
	else
		rotation = ((abs(radian) - maxRotateRadian) * originRotation + maxRotateRadian * targetRotation) / radian;
	// Add 90 degrees (PI/2 radian), since we assume the image is oriented upward.
	head.Rotation = atan2(rotation.y, rotation.x);// + ALLEGRO_PI / 2;
	// Shoot reload.
	reload -= deltaTime;
	if (reload <= 0) {
		// shoot.
		reload = coolDown;
		CreateBullet();
	}
}
