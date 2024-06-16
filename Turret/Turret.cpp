#include <allegro5/color.h>
#include <allegro5/allegro_primitives.h>
#include <cmath>
#include <utility>

#include "Enemy/Enemy.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IObject.hpp"
#include "Engine/IScene.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Point.hpp"
#include "Turret.hpp"

PlayScene* Turret::getPlayScene() {
	return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}
Turret::Turret(std::string imgBase, std::string imgTurret, float x, float y,Team team, float radius, int price, float coolDown, int point) :
	//Sprite(imgTurret, x, y), price(price), coolDown(coolDown), imgBase(imgBase, x, y) {
	Unit(imgTurret, x, y,team,PlayScene::BlockSize/2,100), price(price), imgBase(imgBase, x, y) {
	Unit::coolDown	=	coolDown;
	range = radius;
	if(team==BLUE)
		getPlayScene()->ScorePoint(point);
}
void Turret::Update(float deltaTime) {
	Unit::Update(deltaTime);
	PlayScene* scene = getPlayScene();
	imgBase.Position = Position;
	imgBase.Tint = Tint;
	if (!Enabled)
		return;
	if (Target) {
		Engine::Point diff = Target->Position - Position;
		if (diff.Magnitude() > range) {
			Target->lockedUnits.erase(lockedUnitIterator);
			Target = nullptr;
			lockedUnitIterator = std::list<Unit*>::iterator();
		}
	}
	if (!Target) {
		FindTarget();
	}
	if (Target) {
		Engine::Point originRotation = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
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
		Rotation = atan2(rotation.y, rotation.x) + ALLEGRO_PI / 2;
		// Shoot reload.
		reload -= deltaTime;
		if (reload <= 0) {
			// shoot.
			reload = coolDown;
			CreateBullet();
		}
	}
}
void Turret::Draw(float scale, float cx, float cy, float sx, float sy) const {
	if (Preview) {
		al_draw_filled_circle(
			(Position.x-sx)*scale	+ 	cx,
			(Position.y-sy)*scale 	+	cy,
			range*scale,
			al_map_rgba(0, 255, 0, 50));
	}
	imgBase.Draw(scale, cx, cy, sx, sy);
	Unit::Draw(scale, cx, cy, sx, sy);
	if (PlayScene::DebugMode) {
		// Draw target radius.
		al_draw_circle((Position.x-sx)*scale + cx, (Position.y-sy)*scale + cy, range*scale, al_map_rgb(0, 0, 255), 2);
	}
}
int Turret::GetPrice() const {
	return price;
}
int Turret::GetHp() const{
    return (int)hp;
}
void Turret::Kill()
{
    getPlayScene()->RemoveBuilding(Position.x/PlayScene::BlockSize,Position.y/PlayScene::BlockSize);
	Unit::Kill();
}
