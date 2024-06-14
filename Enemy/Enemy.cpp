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
#include "Enemy.hpp"
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
#include "Engine/Collider.hpp"

// PlayScene *Enemy::getPlayScene()
// {
//     return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
// }
// void Enemy::OnExplode() {
// 	getPlayScene()->EffectGroup->AddNewObject(new ExplosionEffect(Position.x, Position.y));
// 	std::random_device dev;
// 	std::mt19937 rng(dev());
// 	std::uniform_int_distribution<std::mt19937::result_type> distId(1, 3);
// 	std::uniform_int_distribution<std::mt19937::result_type> dist(1, 20);
// 	for (int i = 0; i < 10; i++) {
// 		// Random add 10 dirty effects.
// 		getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-" + std::to_string(distId(rng)) + ".png", dist(rng), Position.x, Position.y));
// 	}
// }
Enemy::Enemy(std::string img, float x, float y,Team team, float radius, float speed, float hp, int money, int point) :
	// Engine::Sprite(img, x, y), speed(speed), hp(hp), money(money), point(point), font(Engine::Resources::GetInstance().GetFont("pirulen.ttf", 32)){
	Unit(img, x, y,team,radius,hp), speed(speed), money(money), point(point){
	// CollisionRadius = radius;
	reachEndTime = 0;
	// for(int i=0;i<STATUS_EFFECT_LENGTH;i++){
	// 	visualEffect[i]	=	nullptr;
	// }
	// ClearEffect();
}
void Enemy::Hit(float damage) {
	Unit::Hit(damage);
	if (hp <= 0) {
		getPlayScene()->ScorePoint(point);
		getPlayScene()->EarnMoney(money);
		// Kill();
	}
}

// void Enemy::Kill(){
// 	OnExplode();
// 	// Remove all turret's reference to target.
// 	for (auto& it: lockedUnits)
// 		it->Target = nullptr;
// 	for (auto& it: lockedBullets)
// 		it->Target = nullptr;
// 	ClearEffect();
// 	getPlayScene()->UnitGroups[team]->RemoveObject(objectIterator);
// 	AudioHelper::PlayAudio("explosion.wav");
// }
void Enemy::UpdatePath(const std::vector<std::vector<int>>& mapDistance) {
	int x = static_cast<int>(floor(Position.x / PlayScene::BlockSize));
	int y = static_cast<int>(floor(Position.y / PlayScene::BlockSize));
	if (x < 0) x = 0;
	if (x >= PlayScene::MapWidth) x = PlayScene::MapWidth - 1;
	if (y < 0) y = 0;
	if (y >= PlayScene::MapHeight) y = PlayScene::MapHeight - 1;
	Engine::Point pos(x, y);
	int num = mapDistance[y][x];
	if (num == -1) {
		num = 0;
		Engine::LOG(Engine::ERROR) << "Enemy path finding error at "<<x<<','<<y<<": "<<-1;
	}
	path = std::vector<Engine::Point>(num + 1);
	while (num != 0) {
		std::vector<Engine::Point> nextHops;
		for (auto& dir : PlayScene::directions) {
			int x = pos.x + dir.x;
			int y = pos.y + dir.y;
			if (x < 0 || x >= PlayScene::MapWidth || y < 0 || y >= PlayScene::MapHeight || mapDistance[y][x] != num - 1)
				continue;
			nextHops.emplace_back(x, y);
		}
		// Choose arbitrary one.
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> dist(0, nextHops.size() - 1);
		pos = nextHops[dist(rng)];
		path[num] = pos;
		num--;
	}
	path[0] = PlayScene::EndGridPoint;
}
void Enemy::Update(float deltaTime) {
	// Pre-calculate the velocity.
	float remainSpeed = speed * deltaTime;
	//Deal with effects
	
	while (remainSpeed != 0) {
		if (path.empty()) {
			// Reach end point.
			//Hit(hp);
			Kill();
			getPlayScene()->Hit();
			reachEndTime = 0;
			return;
		}
		
		Engine::Point nextp = path.back() * PlayScene::BlockSize + Engine::Point(PlayScene::BlockSize / 2, PlayScene::BlockSize / 2);
		Engine::Point vec = nextp - Position;
		
		pathBlock=getPlayScene()->HasBuildingAt(floor(nextp.x/PlayScene::BlockSize),floor(nextp.y/PlayScene::BlockSize));
			
		if(pathBlock){
			Target=pathBlock;
			std::cerr<<"path blocked\n";
			//Point	halfsize	=	Point(PlayScene::BlockSize/2,PlayScene::BlockSize/2);
			//if(Collider::IsCircleNRectOverlap(Position,CollisionRadius,pathBlock->Position-halfsize,pathBlock->Position+halfsize)){
			bool contacted	=	Collider::IsCircleOverlap(Position,CollisionRadius,pathBlock->Position,pathBlock->CollisionRadius);
			if(contacted){
				//Velocity	=	Point(0,0);
				doSpriteUpdate	=	false;
				if(!range){
					if(reload<=0){
						Target->Hit(1);
						reload	=	coolDown;
					}
					reload-=deltaTime;
					std::cerr<<"attacking path blocked\n";
				}
			}
			// if(range && Collider::IsCircleOverlap(Position,CollisionRadius,pathBlock->Position,pathBlock->CollisionRadius)){

			// }

			break;
		}
		else if(!pathBlock) doSpriteUpdate	=	true;

		// Add up the distances:
		// 1. to path.back()
		// 2. path.back() to border
		// 3. All intermediate block size
		// 4. to end point
		reachEndTime = (vec.Magnitude() + (path.size() - 1) * PlayScene::BlockSize - remainSpeed) / speed;
		Engine::Point normalized = vec.Normalize();
		if (remainSpeed - vec.Magnitude() > 0) {
			Position = nextp;
			path.pop_back();
			remainSpeed -= vec.Magnitude();
		}
		else {
			Velocity = normalized * remainSpeed / deltaTime;
			remainSpeed = 0;
		}
	}

	Rotation = atan2(Velocity.y, Velocity.x);
	Unit::Update(deltaTime);
}
// void Enemy::Draw() const {
// 	Sprite::Draw();
// 	if (PlayScene::DebugMode) {
// 		// Draw collision radius.
// 		al_draw_circle(Position.x, Position.y, CollisionRadius, al_map_rgb(255, 0, 0), 2);
// 		//Display health
// 		std::string text = std::to_string((int)hp);
// 		al_draw_text(font.get(),al_map_rgba(255, 255, 255, 127),Position.x,Position.y,0,text.c_str());
// 	}
// }

// void Enemy::GetEffect(StatusEffect newEffect, float timer){
// 	if(!hasStatusEffect[(int)newEffect]){
// 		//effects.push_back(newEffect);
// 		hasStatusEffect[newEffect] = true;
// 		switch (newEffect){
// 			case BURN:
// 				getPlayScene()->EffectGroup->AddNewObject(
// 					visualEffect[BURN]	=	new FireEffect(Position.x, Position.y, this, timer));
					
// 				break;
// 			default:
// 				break;
// 		}	
// 	}
// 	effectTimer[newEffect]	=	timer;
	
// }


// void Enemy::DoEffect(StatusEffect effect, float delta){
// 	switch(effect){
// 		case BURN:
// 			if(effectTimer[BURN] > delta)
// 				Hit(1.0f*delta);
// 			else
// 				Hit(effectTimer[BURN] * delta);
// 			break;
// 		default:
// 			break;
// 	}
// }
// //clear all effect
// void Enemy::ClearEffect(){
// 	for(int i=0;i<STATUS_EFFECT_LENGTH;i++){
// 		ClearEffect((StatusEffect)i);
// 	}
// }
// //clear certain effect
// void Enemy::ClearEffect(StatusEffect effect){
// 	switch (effect){
// 	case BURN:
// 		if(visualEffect[BURN]){
// 			std::cerr<<"remove visual effect at "<<Position.x<<','<<Position.y<<'\n';
// 			visualEffect[BURN]->Parent	=	nullptr;
// 			// std::cerr<<"remove fire effect at "<<Position.x<<','<<Position.y<<'\n';
// 			// getPlayScene()->EffectGroup->RemoveObject(visualEffect[BURN]->GetObjectIterator());
// 			// //delete	visualEffect[BURN];
// 		}
// 		break;
// 	default:
// 		break;
// 	}
// 	hasStatusEffect[effect] = false;
// 	effectTimer[effect]		=	0.0;
// 	visualEffect[effect]	=	nullptr;
// }
