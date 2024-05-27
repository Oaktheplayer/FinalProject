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
#include "VisualEffect.hpp"
#include "Engine/AudioHelper.hpp"

VisualEffect::VisualEffect(std::string img, float x, float y, Engine::Sprite* parent, float duration) :
    Sprite(img, x, y),Parent(parent),timeTicks(0), timeSpan(duration) {};

PlayScene* VisualEffect::getPlayScene() {
	return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}