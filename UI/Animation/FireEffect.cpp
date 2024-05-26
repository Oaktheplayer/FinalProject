#include <cmath>
#include <string>
#include <random>

#include <iostream>

#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IScene.hpp"
#include "Engine/AudioHelper.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Resources.hpp"
#include "FireEffect.hpp"

FireEffect::FireEffect(float x, float y, Engine::Sprite* target, float duration) :
    Sprite("play/fire_particle-1.png", x, y), target(target),timeTicks(0), timeSpan(duration) {
	extingushed =   0;
    
    extinguishTimer =   1.75f;
    for (int i = 1; i <= 15; i++) {
		bmps.push_back(Engine::Resources::GetInstance().GetBitmap("play/fire_particle-" + std::to_string(i) + ".png"));
	}
    std::cerr<<"burn at "<<x<<','<<y<<'\n';
}

PlayScene* FireEffect::getPlayScene() {
	return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}

void FireEffect::Update(float deltaTime)
{
    timeTicks += deltaTime;
    int phase;
    // if(target){
    //     Position    =   target->Position;
    // }else{
    //     getPlayScene()->EffectGroup->RemoveObject(objectIterator);
	// 	return;
    // }
    // if(timeTicks>timeSpan){
    //     if(!extingushed){
    //         extingushed = 1;
    //         timeTicks   =   0;
    //         timeSpan    =   1.75f;
    //     }else{
    //         getPlayScene()->EffectGroup->RemoveObject(objectIterator);return;
    //     }
    // }    
    // if(extingushed){
    //     phase   =   floor(timeTicks*4)+8;
    // }else
    // if(timeTicks>=1){
    //     phase   =   (int)floor((timeTicks-1)*4) % 4 +4;
    // }else{
    //     phase   =   floor(timeTicks*4);
    // }    
    // if(phase>=15){
    //     getPlayScene()->EffectGroup->RemoveObject(objectIterator);return;
    // }
    // bmp = bmps[phase];
    if(timeTicks>=timeSpan || !target){
        std::cerr<<"removed at "<<Position.x<<','<<Position.y<<'\n';
        getPlayScene()->EffectGroup->RemoveObject(objectIterator);return;
    }else{
        Position    =   target->Position;
    }
    phase = ((int)floor(timeTicks /0.25f)) % 4 +4;
    bmp = bmps[phase];
    Sprite::Update(deltaTime);
}