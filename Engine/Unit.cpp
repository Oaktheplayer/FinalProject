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

Unit::Unit(std::string img, float x, float y, float radius, float hp):
    Engine::Sprite(img, x, y), hp(hp),font(Engine::Resources::GetInstance().GetFont("pirulen.ttf", 32)){
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
}

//TODO: How to kill?
//Q1: Remove from where? Enemy? Turret? Do we make two lists or multiple list for each team?


// void Unit::Draw() const {
// 	al_draw_tinted_scaled_rotated_bitmap(bmp.get(), Tint, Anchor.x * GetBitmapWidth(), Anchor.y * GetBitmapHeight(),
// 			Position.x, Position.y, Size.x / GetBitmapWidth()*scale, Size.y / GetBitmapHeight()*scale, Rotation, 0);
// 	if (PlayScene::DebugMode) {
// 		// Draw collision radius.
// 		al_draw_circle(Position.x, Position.y, CollisionRadius, al_map_rgb(255, 0, 0), 2);
// 		//Display health
// 		std::string text = std::to_string((int)hp);
// 		al_draw_text(font.get(),al_map_rgba(255, 255, 255, 127),Position.x,Position.y,0,text.c_str());
// 	}
// }

void Unit::Kill(){};

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
			if(effectTimer[BURN] > delta)
				Hit(1.0f*delta);
			else
				Hit(effectTimer[BURN] * delta);
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

