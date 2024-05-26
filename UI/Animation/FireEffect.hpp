#ifndef FIREEFFECT_HPP
#define FIREEFFECT_HPP
#include <allegro5/bitmap.h>
#include <memory>
#include <vector>

#include "Engine/Sprite.hpp"


class PlayScene;

class FireEffect : public Engine::Sprite {
protected:
	PlayScene* getPlayScene();
	float timeTicks;
	float extinguishTimer = 7.0/4.0;
	std::vector<std::shared_ptr<ALLEGRO_BITMAP>> bmps;
	float timeSpan;
    Engine::Sprite *target;
	bool extingushed = 0;
	std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> fireSound;
public:
	FireEffect(float x, float y, Engine::Sprite *target, float duratioin);
	void Update(float deltaTime) override;
};
#endif // FIREEFFECT_HPP
