#include <allegro5/color.h>
#include <allegro5/allegro_primitives.h>
#include <cmath>
#include <utility>

#include "../Enemy.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IObject.hpp"
#include "Engine/IScene.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Point.hpp"
#include "TroopTurret.hpp"
#include "Bullet/FireBullet.hpp"
#include "Engine/AudioHelper.hpp"

PlayScene* TroopTurret::getPlayScene() {
	return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}
TroopTurret::TroopTurret(std::string img, Enemy* parent, float osx, float osy, float range, float coolDown) :
	//Sprite(, x, y), price(price), coolDown(coolDown), imgBase(imgBase, x, y) {
	Sprite(img,parent->Position.x+osx,parent->Position.y+osy),
	offset(osx,osy),	Parent(parent),
	coolDown(coolDown){
	
}
void TroopTurret::Update(float deltaTime) {
	// Target = Parent->Target;
	// Position = Parent->Position + offset;
}
//TODO: CANT SHOOT
void TroopTurret::RotateTurret(float deltaTime){
	Target	=	Parent->Target;
	Engine::Point originRotation = Engine::Point(cos(Rotation), sin(Rotation));
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
	Rotation = atan2(rotation.y, rotation.x);// + ALLEGRO_PI / 2;
	// Shoot reload.
	reload -= deltaTime;
	if (reload <= 0) {
		// shoot.
		reload = coolDown;
		CreateBullet();
	}
}

void TroopTurret::CreateBullet() {
	Engine::Point diff = Engine::Point(cos(Rotation), sin(Rotation));
	float rotation = atan2(diff.y, diff.x);
	Engine::Point normalized = diff.Normalize();
	// Change bullet position to the front of the gun barrel.
	getPlayScene()->BulletGroup->AddNewObject(new FireBullet(Position + normalized * 36, diff, rotation, Parent));
	AudioHelper::PlayAudio("gun.wav");
}

