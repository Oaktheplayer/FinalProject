#ifndef ENEMY_HPP
#define ENEMY_HPP
#include <list>
#include <vector>
#include <string>

#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
#include	"UI/Component/Label.hpp"
#include "Scene/PlayScene.hpp"

class Bullet;
class PlayScene;
class Turret;

class Enemy : public Engine::Sprite {
protected:
 	std::vector<Engine::Point> path;
	float speed;
	float hp;
	int money;
	int point;
	std::vector<StatusEffect>	effects;
	bool	hasStatusEffect[STATUS_EFFECT_LENGTH];
	float	effectTimer[STATUS_EFFECT_LENGTH];
	void	DoEffect(StatusEffect effect,float delta);
	void	ClearEffect(StatusEffect effect);
	void	ClearEffect();
	std::shared_ptr<ALLEGRO_FONT> 	font;
	// Engine::Label	DebugHp;
	PlayScene* getPlayScene();
	virtual void OnExplode();
public:
	float reachEndTime;
	std::list<Turret*> lockedTurrets;
	std::list<Bullet*> lockedBullets;
	Enemy(std::string img, float x, float y, float radius, float speed, float hp, int money, int point);
 	void Hit(float damage);
	void UpdatePath(const std::vector<std::vector<int>>& mapDistance);
	void Update(float deltaTime) override;
	void Draw() const override;
	void GetEffect(StatusEffect effect, float timer);
};
#endif // ENEMY_HPP
