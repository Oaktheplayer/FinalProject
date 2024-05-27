#ifndef VISUALEFFECT_HPP
#define VISUALEFFECT_HPP
#include <allegro5/bitmap.h>
#include <memory>
#include <vector>

#include "Engine/Sprite.hpp"


class PlayScene;

class VisualEffect : public Engine::Sprite {
protected:
	PlayScene* getPlayScene();
	float timeTicks;
	std::vector<std::shared_ptr<ALLEGRO_BITMAP>> bmps;
	float timeSpan;
public:
	VisualEffect(std::string img, float x, float y, Engine::Sprite *parent, float duratioin);
    Engine::Sprite *Parent = nullptr;	
};
#endif // VISUALEFFECT_HPP
