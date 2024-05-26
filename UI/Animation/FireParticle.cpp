#include <cmath>
#include <string>
#include <random>

#include "FireParticle.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IScene.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Resources.hpp"

PlayScene* FireParticle::getPlayScene() {
	return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}
FireParticle::FireParticle(float x, float y, float vx, float vy) : Sprite("play/fire_particle-1.png", x, y), timeTicks(0) {
	Velocity.x = vx; Velocity.y = vy;
	for (int i = 1; i <= 15; i++) {
		bmps.push_back(Engine::Resources::GetInstance().GetBitmap("play/fire_particle-" + std::to_string(i) + ".png"));
	}
}
void FireParticle::Update(float deltaTime) {
	timeTicks += deltaTime;
	if (timeTicks >= timeSpan) {
		getPlayScene()->EffectGroup->RemoveObject(objectIterator);
		return;
	}
	int phase = floor(timeTicks / timeSpan * bmps.size());
	bmp = bmps[phase];
	Sprite::Update(deltaTime);
}