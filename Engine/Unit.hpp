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

//extern float scale;

class Bullet;
class PlayScene;

class Unit: public Sprite{
protected:    
	float hp;
	Team team;

	PlayScene* getPlayScene();
	virtual void OnExplode();
    VisualEffect*	visualEffect[STATUS_EFFECT_LENGTH];
	bool	hasStatusEffect[STATUS_EFFECT_LENGTH];
	float	effectTimer[STATUS_EFFECT_LENGTH];
	void	DoEffect(StatusEffect effect,float delta);
	void	ClearEffect(StatusEffect effect);
	void	ClearEffect();
    
	std::shared_ptr<ALLEGRO_FONT> 	font;
    virtual void CreateBullet();
public:
    Unit(std::string img, float x, float y,Team team, float radius, float hp);
	Team	getTeam();
    virtual void Hit(float damage);
    std::list<Unit*> lockedUnits;
	std::list<Bullet*> lockedBullets;
	virtual void Kill();
	//void Update(float deltaTime) override;
	void Draw(float scale=1, float cx=0, float cy=0, float sx=0, float sy=0) const override;
	void GetEffect(StatusEffect effect, float timer);
};

#endif //UNIT_HPP