#include "Bullet.hpp"
#include "Engine/Collider.hpp"
#include "Enemy/Enemy.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IObject.hpp"
#include "Engine/IScene.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
#include "Engine/Unit.hpp"

#include <iostream>

PlayScene* Bullet::getPlayScene() {
	return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}
void Bullet::OnExplode(Enemy* enemy) {
}
void Bullet::GiveEffect(Enemy* enemy){
	//TODO: make custom effect time. For now it's always 6 seconds
	int i=0;
	int len =	effectOnEnemy.size();
	for(int i=0;i<len;i++){
		enemy->GetEffect(effectOnEnemy[i], 6.0);
	}
}
Bullet::Bullet(std::string img, float speed, float damage, Engine::Point position, Engine::Point forwardDirection, float rotation, Unit *parent, std::vector<StatusEffect> effects) : 
	Sprite(img, position.x, position.y), speed(speed), damage(damage), parent(parent), effectOnEnemy(effects){
    Velocity = forwardDirection.Normalize() * speed;
	Rotation = rotation;
	team	 = parent->getTeam();
	CollisionRadius = 4;
	std::cerr<<"Bullet:\tstill good\n";
}
Bullet::Bullet(std::string img, float speed, float damage, Engine::Point position, Engine::Point forwardDirection, float rotation, Unit* parent) :
	Sprite(img, position.x, position.y), speed(speed), damage(damage), parent(parent) {
	Velocity = forwardDirection.Normalize() * speed;
	Rotation = rotation;
	team	 = parent->getTeam();
	CollisionRadius = 4;
	std::cerr<<"Bullet:\tstill good\n";
	
}
void Bullet::Update(float deltaTime) {
	Sprite::Update(deltaTime);
	PlayScene* scene = getPlayScene();
	// Can be improved by Spatial Hash, Quad Tree, ...
	// However simply loop through all enemies is enough for this program.
	for (int i=0;i<TEAM_COUNT;i++){
		if(i==team)	continue;
		Engine::Group*	enemyGroup	=	scene->UnitGroups[i];
		for (auto& it : enemyGroup->GetObjects()) {
			Enemy* enemy = dynamic_cast<Enemy*>(it);
			if (!enemy->Visible)
				continue;
			if (Engine::Collider::IsCircleOverlap(Position, CollisionRadius, enemy->Position, enemy->CollisionRadius)) {
				// if(!explosionRadius){
					OnExplode(enemy);
					GiveEffect(enemy);
					enemy->Hit(damage);
				// }else
				// {
				// 	for (auto& that : scene->EnemyGroup->GetObjects()){
				// 		Enemy* enemy2 = dynamic_cast<Enemy*>(that);
				// 		if (!enemy2->Visible)
				// 			continue;
				// 		if (Engine::Collider::IsCircleOverlap(Position, explosionRadius, enemy2->Position, enemy2->CollisionRadius))	{
				// 			OnExplode(enemy2);
				// 			GiveEffect(enemy2);
				// 			enemy2->Hit(damage);
				// 		}
				// 	}
				// }
				getPlayScene()->BulletGroup->RemoveObject(objectIterator);
				return;
			}
		}
	}
	// Check if out of boundary.
	if (!Engine::Collider::IsRectOverlap(Position - Size / 2, Position + Size / 2, Engine::Point(0, 0), PlayScene::GetClientSize()))
		getPlayScene()->BulletGroup->RemoveObject(objectIterator);
}
