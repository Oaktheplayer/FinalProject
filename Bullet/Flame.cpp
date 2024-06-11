#include <allegro5/base.h>
#include <random>
#include <string>

#include "UI/Animation/DirtyEffect.hpp"
#include "Enemy/Enemy.hpp"
#include "Flame.hpp"
#include "Engine/Group.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/Unit.hpp"

class Turret;

Flame::Flame(Engine::Point position, Engine::Point forwardDirection, float rotation, Unit* parent, std::vector<StatusEffect> effects) :
	Bullet("play/fire_particle-10.png", 100, 0, position, forwardDirection, rotation - ALLEGRO_PI / 2, parent, effects) {
		explosionRadius	=	24;
}
void Flame::OnExplode(Unit* enemy) {
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(2, 5);
	getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-1.png", dist(rng), enemy->Position.x, enemy->Position.y));
}

