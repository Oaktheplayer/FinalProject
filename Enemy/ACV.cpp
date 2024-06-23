#include <allegro5/base.h>
#include <random>
#include <string>
#include "Engine/Point.hpp"
#include "ACV.hpp"
#include "Engine/AudioHelper.hpp"
#include "Bullet/FireBullet.hpp"
#include <iostream>

const int ACV::Price = 25;
const int ACV::TurretRange	=	300;

ACV::ACV(int x, int y,Team team) : Enemy("play/acv_troop.png", x, y,team, 20, 20, 75, 50, 5,Price),
	head("play/acv_troop-head.png", this,0,0,TurretRange,1), targetRotation(0),
	turretOffset(Point(-8,0)){
	coolDown	=	0.2;
	range	=	TurretRange;
	type	=	Amph;
}
void ACV::Draw(float scale, float cx, float cy, float sx, float sy) const {
	Enemy::Draw(scale, cx, cy, sx, sy);
	head.Draw(scale, cx, cy, sx, sy);
}
void ACV::Update(float deltaTime) {
	head.Position = Position+turretOffset.Rotate(Rotation);
	if(!Enabled) return;
	ShootTarget(deltaTime);
	if(!Target) head.Rotation	=	Rotation;
    Enemy::Update(deltaTime);
}

void ACV::CreateBullet() {
	Engine::Point diff = Engine::Point(cos(head.Rotation), sin(head.Rotation));
	float rotation = atan2(diff.y, diff.x);
	Engine::Point normalized = diff.Normalize();
	// Change bullet position to the front of the gun barrel.
	getPlayScene()->BulletGroup->AddNewObject(new FireBullet(Position + normalized * 36, diff, rotation, this));
	AudioHelper::PlayAudio("gun.wav");
}
void ACV::Kill()
{
	Enemy::Kill();
}
// TODO: Head rotation error
void ACV::RotateHead(float deltaTime){
	head.RotateTurret(deltaTime);
    //std::cerr<<"rotate head\n";
}