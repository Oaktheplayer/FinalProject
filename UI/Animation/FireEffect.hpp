#ifndef FIREEFFECT_HPP
#define FIREEFFECT_HPP
#include <allegro5/bitmap.h>
#include <memory>
#include <vector>

#include "Engine/Sprite.hpp"
#include	"UI/Animation/VisualEffect.hpp"


//class PlayScene;

class FireEffect : public VisualEffect {
protected:
	float extinguishTimer = 7.0/4.0;
	bool extingushed = 0;
	std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> fireSound;
public:
	FireEffect(float x, float y, Engine::Sprite *parent, float duratioin);
	~FireEffect();
	void Update(float deltaTime) override;
};
#endif // FIREEFFECT_HPP
