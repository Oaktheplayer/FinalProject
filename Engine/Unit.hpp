#ifndef UNIT_HPP
#define UNIT_HPP
#include <allegro5/color.h>
#include <list>
#include <vector>
#include <string>

#include "UI/Component/Image.hpp"
#include "UI/Animation/VisualEffect.hpp"
#include "UI/Component/Label.hpp"
#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
//#include "Scene/PlayScene.hpp"

using namespace Engine;

//extern float scale;

class Bullet;
class PlayScene;
enum Team{
    BLUE,
    RED,
    TEAM_COUNT
};
typedef enum effects{
    BURN,
    STATUS_EFFECT_LENGTH
}StatusEffect;
class Unit: public Sprite{
protected:    
	float hp;
	Team team;

	int price;

	float range = 0;
    float reload = 0;
    float coolDown=1;
    float rotateRadian = 2 * ALLEGRO_PI;
	std::list<Unit*>::iterator lockedUnitIterator;
	bool doSpriteUpdate = 1;

	PlayScene* getPlayScene();
	virtual void OnExplode();
    VisualEffect*	visualEffect[STATUS_EFFECT_LENGTH];
	bool	hasStatusEffect[STATUS_EFFECT_LENGTH];
	float	effectTimer[STATUS_EFFECT_LENGTH];
	void	DoEffect(StatusEffect effect,float delta);
	void	ClearEffect(StatusEffect effect);
	void	ClearEffect();
    
	std::shared_ptr<ALLEGRO_FONT> 	font;
	void FindTarget();
	void RemoveTarget();
	void ShootTarget(float deltaTime);
	virtual void RotateHead(float deltaTime);
    virtual void CreateBullet();
public:
    bool Enabled = true;
    bool Preview = false;
    Unit(std::string img, float x, float y,Team team, float radius, float hp, int price);
	Team	getTeam();
    Unit* 	Target = nullptr;
    virtual void Hit(float damage);
    std::list<Unit*> lockedUnits;
	std::list<Bullet*> lockedBullets;
	virtual void Kill();
	void Update(float deltaTime) override;
	void Draw(float scale=1, float cx=0, float cy=0, float sx=0, float sy=0) const override;
	void GetEffect(StatusEffect effect, float timer);
    int  GetPrice() const;
	int  GetHp() const;
};

#endif //UNIT_HPP