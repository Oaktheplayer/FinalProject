#ifndef ENEMY_HPP
#define ENEMY_HPP
#include <list>
#include <vector>
#include <queue>
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
 	// std::vector<Engine::Point> path;
	std::queue<Engine::Point> path;
	Turret* pathBlock=nullptr;
	float speed;
	int money;
	int point;
    int price;

public:
	float reachEndTime;

	Enemy(std::string img, float x, float y, Team team, float radius, float speed, float hp, int money, int point ,int price);
 	void Hit(float damage) override;
	// virtual void Kill() override;
	void UpdatePath(const std::vector<std::vector<int>>& mapDistance);
	void Update(float deltaTime) override;
    void Draw(float scale=1, float cx=0, float cy=0, float sx=0, float sy=0) const override;
    int GetPrice() const override;
	// void Draw() const override;
	// void GetEffect(StatusEffect effect, float timer);
};
#endif // ENEMY_HPP
