#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Bullet/Flame.hpp"
#include "Engine/Group.hpp"
#include "Flamethrower.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Point.hpp"

const int Flamethrower::Price = 150;
Flamethrower::Flamethrower(float x, float y, Team team) :
	Turret("play/tower-base.png", "play/turret-6.png", x, y, team, 75, Price, 0.1, 20) {
	// Move center downward, since we the turret head is slightly biased upward.
	effectOnEnemy.push_back(BURN);
	Anchor.y += 8.0f / GetBitmapHeight();
}
void Flamethrower::CreateBullet() {
	Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
	float rotation = atan2(diff.y, diff.x);
	Engine::Point normalized = diff.Normalize();
	// Change bullet position to the front of the gun barrel.
	getPlayScene()->BulletGroup->AddNewObject(new Flame(Position + normalized * 36, diff, rotation, this, this->effectOnEnemy));
	AudioHelper::PlayAudio("flame_thrower.mp3");
}
