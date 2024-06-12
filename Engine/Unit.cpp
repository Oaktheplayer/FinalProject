#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <cmath>
#include <random>
#include <string>
#include <vector>
#include <allegro5/allegro_font.h>

#include "Engine/AudioHelper.hpp"
#include "Bullet/Bullet.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "Unit.hpp"
#include "UI/Animation/ExplosionEffect.hpp"
#include "UI/Animation/FireParticle.hpp"
#include "UI/Animation/FireEffect.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IScene.hpp"
#include "Engine/LOG.hpp"
#include "Scene/PlayScene.hpp"
#include "Turret/Turret.hpp"
#include "Engine/Resources.hpp"

//extern
//float scale;

PlayScene *Unit::getPlayScene()
{
    return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}

void Unit::OnExplode() {
	getPlayScene()->EffectGroup->AddNewObject(new ExplosionEffect(Position.x, Position.y));
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> distId(1, 3);
	std::uniform_int_distribution<std::mt19937::result_type> dist(1, 20);
	for (int i = 0; i < 10; i++) {
		// Random add 10 dirty effects.
		getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-" + std::to_string(distId(rng)) + ".png", dist(rng), Position.x, Position.y));
	}
}

Unit::Unit(std::string img, float x, float y,Team team, float radius, float hp):
    Engine::Sprite(img, x, y), hp(hp),team(team),font(Engine::Resources::GetInstance().GetFont("pirulen.ttf", 32)){
    CollisionRadius = radius;
	for(int i=0;i<STATUS_EFFECT_LENGTH;i++){
		visualEffect[i]	=	nullptr;
	}
	ClearEffect();
	//std::cerr<<scale<<'\n';
}

//Reduce hp. THAT'S ALL IT DOES.
void Unit::Hit(float damage) {
	hp -= damage;
	if(hp<=0){
		Kill();
	}
}

//TODO: How to kill?
//Q1: Remove from where? Enemy? Turret? Do we make two lists or multiple list for each team?
void Unit::Kill(){
	OnExplode();
	// Remove all turret's reference to target.
	for (auto& it: lockedUnits)
		it->Target = nullptr;
	for (auto& it: lockedBullets)
		it->Target = nullptr;
	ClearEffect();
	getPlayScene()->UnitGroups[team]->RemoveObject(objectIterator);
	AudioHelper::PlayAudio("explosion.wav");
}

void Unit::Draw(float scale, float cx, float cy, float sx, float sy) const {
	Sprite::Draw(scale,cx,cy,sx,sy);
	if (PlayScene::DebugMode) {
		// Draw collision radius.
		al_draw_circle(
			(Position.x-sx)*scale	+ 	cx,
			(Position.y-sy)*scale 	+	cy,
			CollisionRadius*scale,
			al_map_rgb(255, 0, 0), 2);
		//Display health
		std::string text = std::to_string((int)hp);
		al_draw_text(font.get(),al_map_rgba(255, 255, 255, 127),
			(Position.x-sx)*scale	+ 	cx,
			(Position.y-sy)*scale 	+	cy,
			0,text.c_str());
	}
}

void Unit::CreateBullet(){}

void Unit::RemoveTarget(){
	Engine::Point diff = Target->Position - Position;
	if (diff.Magnitude() > range) {
		Target->lockedUnits.erase(lockedUnitIterator);
		Target = nullptr;
		lockedUnitIterator = std::list<Unit*>::iterator();
	}
}
void Unit::FindTarget(){
	PlayScene* scene = getPlayScene();
	for (int i=0;i<TEAM_COUNT;i++){
		if(i==team)	continue;
		Engine::Group*	enemyGroup	=	scene->UnitGroups[i];
		for (auto& it : enemyGroup->GetObjects()) {
			Engine::Point diff = it->Position - Position;
			if (diff.Magnitude() <= range) {
				Target = dynamic_cast<Unit*>(it);
				Target->lockedUnits.push_back(this);
				lockedUnitIterator = std::prev(Target->lockedUnits.end());
				break;
			}
		}
	}
}

void Unit::ShootTarget(float deltaTime){
	PlayScene* scene = getPlayScene();
	if (Target) {
		Engine::Point diff = Target->Position - Position;
		if (diff.Magnitude() > range) {
			Target->lockedUnits.erase(lockedUnitIterator);
			Target = nullptr;
			lockedUnitIterator = std::list<Unit*>::iterator();
		}
	}
	if (!Target) {
		// Lock first seen target.
		// Can be improved by Spatial Hash, Quad Tree, ...
		// However simply loop through all enemies is enough for this program.
		for (int i=0;i<TEAM_COUNT;i++){
			if(i==team)	continue;
			Engine::Group*	enemyGroup	=	scene->UnitGroups[i];
			for (auto& it : enemyGroup->GetObjects()) {
				Engine::Point diff = it->Position - Position;
				if (diff.Magnitude() <= range) {
					Target = dynamic_cast<Unit*>(it);
					Target->lockedUnits.push_back(this);
					lockedUnitIterator = std::prev(Target->lockedUnits.end());
					break;
				}
			}
		}
	}
	if (Target) {
		RotateHead(deltaTime);
		// if (reload <= 0) {
		// 	// shoot.
		// 	reload = coolDown;
		// 	CreateBullet();
		// }
	}
};

void Unit::RotateHead(float deltaTime){};
//TODO: write "Kill()" so that turrets can die
void Unit::GetEffect(StatusEffect newEffect, float timer){
	if(!hasStatusEffect[(int)newEffect]){
		//effects.push_back(newEffect);
		hasStatusEffect[newEffect] = true;
		switch (newEffect){
			case BURN:
				getPlayScene()->EffectGroup->AddNewObject(
					visualEffect[BURN]	=	new FireEffect(Position.x, Position.y, this, timer));
					
				break;
			default:
				break;
		}	
	}
	effectTimer[newEffect]	=	timer;
	
}


void Unit::DoEffect(StatusEffect effect, float delta){
	switch(effect){
		case BURN:
			Hit(1.0f*delta);
			break;
		default:
			break;
	}
}
//clear all effect
void Unit::ClearEffect(){
	for(int i=0;i<STATUS_EFFECT_LENGTH;i++){
		ClearEffect((StatusEffect)i);
	}
}
//clear certain effect
void Unit::ClearEffect(StatusEffect effect){
	switch (effect){
	case BURN:
		if(visualEffect[BURN]){
			std::cerr<<"remove visual effect at "<<Position.x<<','<<Position.y<<'\n';
			visualEffect[BURN]->Parent	=	nullptr;
			// std::cerr<<"remove fire effect at "<<Position.x<<','<<Position.y<<'\n';
			// getPlayScene()->EffectGroup->RemoveObject(visualEffect[BURN]->GetObjectIterator());
			// //delete	visualEffect[BURN];
		}
		break;
	default:
		break;
	}
	hasStatusEffect[effect] = false;
	effectTimer[effect]		=	0.0;
	visualEffect[effect]	=	nullptr;
}

Team Unit::getTeam(){return	team;}
