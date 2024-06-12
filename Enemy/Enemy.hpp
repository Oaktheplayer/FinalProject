#ifndef ENEMY_HPP
#define ENEMY_HPP
#include <list>
#include <vector>
#include <string>

#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
#include	"UI/Component/Label.hpp"
#include	"UI/Animation/VisualEffect.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Unit.hpp"

class Bullet;
class PlayScene;
class Turret;

//extern float scale;

class Enemy : public Unit {
protected:
 	std::vector<Engine::Point> path;
	float speed;
	// float hp;
	int money;
	int point;
	// VisualEffect*	visualEffect[STATUS_EFFECT_LENGTH];
	// bool	hasStatusEffect[STATUS_EFFECT_LENGTH];
	// float	effectTimer[STATUS_EFFECT_LENGTH];
	// void	DoEffect(StatusEffect effect,float delta);
	// void	ClearEffect(StatusEffect effect);
	// void	ClearEffect();
	// std::shared_ptr<ALLEGRO_FONT> 	font;
	// PlayScene* getPlayScene();
	// virtual void OnExplode();
public:
	float reachEndTime;
	Enemy(std::string img, float x, float y, Team team, float radius, float speed, float hp, int money, int point);
 	void Hit(float damage) override;
	// virtual void Kill() override;
	void UpdatePath(const std::vector<std::vector<int>>& mapDistance);
	void Update(float deltaTime) override;
	// void Draw() const override;
	// void GetEffect(StatusEffect effect, float timer);
};
#endif // ENEMY_HPP
