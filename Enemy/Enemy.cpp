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

Enemy::Enemy(std::string img, float x, float y,Team team, float radius, float speed, float hp, int money, int point) :
	Unit(img, x, y,team,radius,hp), speed(speed), money(money), point(point){
	reachEndTime = 0;
}
void Enemy::Hit(float damage) {
	Unit::Hit(damage);
	if (hp <= 0) {
		getPlayScene()->ScorePoint(point);
		getPlayScene()->EarnMoney(money);
		// Kill();
	}
}

void Enemy::UpdatePath(const std::vector<std::vector<int>>& mapDistance) {
	int x = static_cast<int>(floor(Position.x / PlayScene::BlockSize));
	int y = static_cast<int>(floor(Position.y / PlayScene::BlockSize));
	if (x < 0) x = 0;
	if (x >= getPlayScene()->MapWidth) x = getPlayScene()->MapWidth - 1;
	if (y < 0) y = 0;
	if (y >= getPlayScene()->MapHeight) y = getPlayScene()->MapHeight - 1;
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
			if (x < 0 || x >= getPlayScene()->MapWidth || y < 0 || y >= getPlayScene()->MapHeight || mapDistance[y][x] != num - 1)
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
	path[0] = getPlayScene()->EndGridPoint;
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
			bool contacted	=	Collider::IsCircleOverlap(Position,CollisionRadius,pathBlock->Position,pathBlock->CollisionRadius);
			if(contacted){
				doSpriteUpdate	=	false;
				if(!range){
					if(reload<=0){
						Target->Hit(1);
						reload	=	coolDown;
					}
					reload-=deltaTime;
					std::cerr<<"attacking building\n";
				}
                break;
			}
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
