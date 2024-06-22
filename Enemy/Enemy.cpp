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
#include "Building/Building.hpp"

Enemy::Enemy(std::string img, float x, float y,Team team, float radius, float speed, float hp, int money, int point,int price) :
	Unit(img, x, y,team,radius,hp,price), speed(speed), money(money), point(point){
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
	if(type	!=Air){
		int x = static_cast<int>(floor(Position.x / PlayScene::BlockSize));
		int y = static_cast<int>(floor(Position.y / PlayScene::BlockSize));
		auto scene = getPlayScene();
		if(Point(x,y)!=scene->SpawnGridPoint){
			if (x < 0) x = 0;
			if (x >= scene->MapWidth) x = scene->MapWidth - 1;
			if (y < 0) y = 0;
			if (y >= scene->MapHeight) y = scene->MapHeight - 1;
		}
		Engine::Point pos(x, y);
		path		=	std::queue<Point>();
		roadBlockQ	=	std::queue<Point>();
		std::string	path_str = std::string(scene->AStarPathFinding(pos));
		Point	nextp =	pos;
		for(char i: path_str){
			Point dir(PlayScene::directions[(int)i-'0']);
			scene->mapDirection[nextp.y][nextp.x]	=	i-'0';
			if(scene->HasBuildingAt(nextp.x,nextp.y)){
				roadBlockQ.push(nextp);
			}
			nextp=nextp+dir;
			path.push(nextp);
		}
        if(getPlayScene()->HasBuildingAt(nextp.x,nextp.y))roadBlockQ.push(nextp);
	}else{
		AirTroopTargetFinding();
	}
}
void Enemy::Update(float deltaTime) {
	// Pre-calculate the velocity.
	if(!Enabled) return;
	if(type	!= Air){
		float remainSpeed = speed * deltaTime;
		while (remainSpeed != 0) {
			if (Position.x==getPlayScene()->EndGridPoint.x * PlayScene::BlockSize +  PlayScene::BlockSize / 2 &&
                Position.y==getPlayScene()->EndGridPoint.y * PlayScene::BlockSize +  PlayScene::BlockSize / 2 &&
                !getPlayScene()->BaseExsist) {
				// Reach end point.
				//Hit(hp);
				Kill();
				getPlayScene()->Hit();
				reachEndTime = 0;
				return;
			}
			Engine::Point nextp = path.front() * PlayScene::BlockSize + Engine::Point(PlayScene::BlockSize / 2, PlayScene::BlockSize / 2);
			Engine::Point vec = nextp - Position;
			//roadBlock=getPlayScene()->HasBuildingAt(floor(nextp.x/PlayScene::BlockSize),floor(nextp.y/PlayScene::BlockSize));

			if(!roadBlockQ.empty()){
				if(!(roadBlock	=	getPlayScene()->HasBuildingAt(roadBlockQ.front()))){
					roadBlockQ.pop();
				}
			}
			else roadBlock	=	nullptr;

			if(roadBlock){
				if(roadBlock!=Target){
					if(range){	
						Engine::Point diff = roadBlock->Position - Position;
						if (diff.Magnitude() <= range) {
							if(Target){
								Target->lockedUnits.erase(lockedUnitIterator);
								Target = nullptr;
								lockedUnitIterator = std::list<Unit*>::iterator();
							}
							Target = dynamic_cast<Unit*>(roadBlock);
							Target->lockedUnits.push_back(this);
							lockedUnitIterator = std::prev(Target->lockedUnits.end());
							// Target	=	roadBlock;
							// RotateHead(deltaTime);
						}
						// if(Target && roadBlock==Target)  doSpriteUpdate	=	false;
						// else doSpriteUpdate	=	true;
					}
					else{
						if(Collider::IsCircleOverlap(Position,CollisionRadius,roadBlock->Position,roadBlock->CollisionRadius)){
							if(roadBlock!=Target){
								Target=roadBlock;
							}

						}
					}
				}
				if(roadBlock==Target){
    	        	doSpriteUpdate	=	false;
					if(!range){
						if(reload<=0){
							Target->Hit(1);
							reload	=	coolDown;
						}
						reload-=deltaTime;
					}
					break;
				}
				else doSpriteUpdate	=	true;
			}
			else doSpriteUpdate	=	true;
			//std::cerr<<"done raodblock update.\n";
			// Add up the distances:
			// 1. to path.back()
			// 2. path.back() to border
			// 3. All intermediate block size
			// 4. to end point
			reachEndTime = (vec.Magnitude() + (path.size() - 1) * PlayScene::BlockSize - remainSpeed) / speed;
			Engine::Point normalized = vec.Normalize();
			if (remainSpeed - vec.Magnitude() > 0) {
				Position = nextp;
				path.pop();
				remainSpeed -= vec.Magnitude();
			}
			else {
				Velocity = normalized * remainSpeed / deltaTime;
				remainSpeed = 0;
			}
		}
		Rotation = atan2(Velocity.y, Velocity.x);
	}
	else{
		if(!AirTroopUpdate(deltaTime)){
			return;
		}
	}
	Unit::Update(deltaTime);
}

void Enemy::Draw(float scale, float cx, float cy, float sx, float sy) const {
    if (Preview) {
        al_draw_filled_circle(
                (Position.x-sx)*scale	+ 	cx,
                (Position.y-sy)*scale 	+	cy,
                range*scale,
                al_map_rgba(0, 255, 0, 50));
    }
    Unit::Draw(scale, cx, cy, sx, sy);
}

void Enemy::AirTroopTargetFinding(){}
bool Enemy::AirTroopUpdate(float deltaTime){return false;}
