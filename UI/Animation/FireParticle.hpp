#ifndef FIREPARTICLE_HPP
#define FIREPARTICLE_HPP
#include <allegro5/bitmap.h>
#include <memory>
#include <vector>

#include "Engine/Sprite.hpp"

class PlayScene;

class FireParticle : public Engine::Sprite {
protected:
	PlayScene* getPlayScene();
	float timeTicks;
	std::vector<std::shared_ptr<ALLEGRO_BITMAP>> bmps;
	float timeSpan = 5;
public:
	FireParticle(float x, float y, float vx, float vy);
	void Update(float deltaTime) override;
};
#endif // FIREPARTICLE_HPP
