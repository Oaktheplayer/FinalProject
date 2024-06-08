#ifndef UNIT_HPP
#define UNIT_HPP
#include <allegro5/color.h>
#include <list>
#include <vector>
#include <string>

#include "UI/Component/Image.hpp"
#include	"UI/Animation/VisualEffect.hpp"
#include	"UI/Component/Label.hpp"
#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
#include "Scene/PlayScene.hpp"

using namespace Engine;


class Bullet;
class PlayScene;

class Unit: public Sprite{
protected:    
	float hp;

	PlayScene* getPlayScene();
	virtual void OnExplode();
    VisualEffect*	visualEffect[STATUS_EFFECT_LENGTH];
	bool	hasStatusEffect[STATUS_EFFECT_LENGTH];
	float	effectTimer[STATUS_EFFECT_LENGTH];
	void	DoEffect(StatusEffect effect,float delta);
	void	ClearEffect(StatusEffect effect);
	void	ClearEffect();
    
	std::shared_ptr<ALLEGRO_FONT> 	font;
public:
    Unit(std::string img, float x, float y, float radius, float hp);
    void Hit(float damage);
	//virtual void Kill();
	//void UpdatePath(const std::vector<std::vector<int>>& mapDistance);
	//void Update(float deltaTime) override;
	void Draw() const override;
	void GetEffect(StatusEffect effect, float timer);
};

#endif //UNIT_HPP