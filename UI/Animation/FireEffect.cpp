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
#include "UI/Animation/VisualEffect.hpp"
#include "Engine/AudioHelper.hpp"

FireEffect::FireEffect(float x, float y, Engine::Sprite* parent, float duration) :
    VisualEffect::VisualEffect("play/fire_particle-1.png", x, y,parent,duration) {
	extingushed =   0;
    extinguishTimer =   1.75f;
    for (int i = 1; i <= 15; i++) {
		bmps.push_back(Engine::Resources::GetInstance().GetBitmap("play/fire_particle-" + std::to_string(i) + ".png"));
	}
    //fireSound   =   AudioHelper::PlaySample("burning.mp3");
}

FireEffect::~FireEffect(){
    std::cerr<<"removed at\t"<<Position.x<<','<<Position.y<<'\n';
    //AudioHelper::StopSample(fireSound);
}

void    FireEffect::Update(float deltaTime)
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
    if(timeTicks>=timeSpan || !Parent){
        //std::cerr<<"removed at\t"<<Position.x<<','<<Position.y<<'\n';
        getPlayScene()->EffectGroup->RemoveObject(objectIterator);
        return;
    }else{
        Position    =   Parent->Position;
        
    }
    phase = ((int)floor(timeTicks /0.25f)) % 4 +4;
    bmp = bmps[phase];
    Sprite::Update(deltaTime);
}