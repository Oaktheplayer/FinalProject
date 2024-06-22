#include <string>
#include <iostream>
#include <math.h>
#include "PlaneEnemy.hpp"
#include "Engine/Collider.hpp"
#include "Bullet/FireBullet.hpp"
#include "Engine/AudioHelper.hpp"
const int PlaneEnemy::Price = 50;
PlaneEnemy::PlaneEnemy(int x, int y, Team team) : Enemy("play/enemy-2.png", x, y,team, 16, 100, 10, 10, 3 ,Price) {
	// Use bounding circle to detect collision is for simplicity, pixel-perfect collision can be implemented quite easily,
	// and efficiently if we use AABB collision detection first, and then pixel-perfect collision.
	Target	=	nullptr;
	type	=	Air;
	toEnd	=	false;
	rotateRadian	=	0.5*ALLEGRO_PI;
	escapeCoolDown	=5;
	coolDown	=	0.5;
}

void PlaneEnemy::AirTroopTargetFinding(){
	std::cerr<<"start Air find target\n";
	PlayScene *scene = getPlayScene();

	if (Target) {
		Target->lockedUnits.erase(lockedUnitIterator);
		Target = nullptr;
		lockedUnitIterator = std::list<Unit*>::iterator();
		escapeCoolDown	=	0.5;
	}

	float curDis = -1;
	for (int i=0;i<TEAM_COUNT;i++){
		if(i==team)	continue;
		Engine::Group*	enemyGroup	=	scene->UnitGroups[i];
		for (auto& it : enemyGroup->GetObjects()) {
			Engine::Point diff = it->Position - Position;
			float	dis = diff.Magnitude();
			std::cerr<<it->Position.x<<','<<it->Position.y<<": "<< dis<<'\n';
			if (curDis==-1 || dis < curDis) {
				Target = dynamic_cast<Unit*>(it);
				curDis	=	dis;
				// break;
			}
		}
	}
	if(Target){
		std::cerr<<"found Target: "<<Target->Position.x<<','<<Target->Position.y<<'\n';
				Target->lockedUnits.push_back(this);
				lockedUnitIterator = std::prev(Target->lockedUnits.end());
		targetPos	=	Target->Position;
		toEnd	=	false;
	}
	else{
		targetPos=getPlayScene()->EndGridPoint*PlayScene::BlockSize+Point(PlayScene::BlockSize/2,PlayScene::BlockSize/2);
		toEnd	=	true;
	}

	
	//
	std::cerr<<"end Air find target\n";
}

bool PlaneEnemy::AirTroopUpdate(float deltaTime)
{	
	if ((int)abs(Position.x- (getPlayScene()->EndGridPoint.x * PlayScene::BlockSize +  PlayScene::BlockSize / 2))<4 &&
    	(int)abs(Position.y- (getPlayScene()->EndGridPoint.y * PlayScene::BlockSize +  PlayScene::BlockSize / 2))<4) {
		// Reach end point.
		//Hit(hp);
		Kill();
		getPlayScene()->Hit();
		reachEndTime = 0;
		return true;
	}
	if(escapeReload<=0){
		escapeReload	=	0;
	}else{
		escapeReload-=deltaTime;
		return false;
	}
	if(Target && Engine::Collider::IsCircleOverlap(Position,CollisionRadius,Target->Position,Target->CollisionRadius)){
		escapeReload	=	escapeCoolDown;
		return false;
	}

	if(!Target && !toEnd)	{
		AirTroopTargetFinding();
		if(!Target){
			targetPos=getPlayScene()->EndGridPoint*PlayScene::BlockSize+Point(PlayScene::BlockSize/2,PlayScene::BlockSize/2);
			toEnd	=	true;
		}else
			targetPos=Target->Position;
		std::cerr<<targetPos.x<<','<<targetPos.y<<'\n';
	}
	Engine::Point originRotation = Engine::Point(cos(Rotation), sin(Rotation));
	Engine::Point targetRotation = (targetPos - Position).Normalize();
	float cosTheta = originRotation.Dot(targetRotation);
	if (cosTheta >= 1 || abs(1-cosTheta)<0.01){
		Velocity=targetRotation*speed;
		Rotation = atan2(Velocity.y, Velocity.x);
		if(Target){
			reload -= deltaTime;
			if (reload <= 0) {
				// shoot.
				reload = coolDown;
				CreateBullet();
			}
		}
	}
	else{
		Point perpRotation(-originRotation.y,originRotation.x);
		if(perpRotation.Dot(targetRotation)>0){
			Rotation+=rotateRadian*deltaTime;
			if(Rotation>ALLEGRO_PI) Rotation-=2*ALLEGRO_PI;
		}else
		{
			Rotation-=rotateRadian*deltaTime;
			if(Rotation<-ALLEGRO_PI) Rotation+=2*ALLEGRO_PI;
		}
		Velocity.x=cos(Rotation)*speed;
		Velocity.y=sin(Rotation)*speed;
	}
	

    return false;
}

void PlaneEnemy::CreateBullet() {
	Engine::Point diff = Engine::Point(cos(Rotation), sin(Rotation));
	float rotation = atan2(diff.y, diff.x);
	Engine::Point normalized = diff.Normalize();
	// Change bullet position to the front of the gun barrel.
	getPlayScene()->BulletGroup->AddNewObject(new FireBullet(Position + normalized * 36, diff, rotation, this));
	AudioHelper::PlayAudio("gun.wav");
}
