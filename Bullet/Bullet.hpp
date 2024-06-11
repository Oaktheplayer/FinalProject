#ifndef BULLET_HPP
#define BULLET_HPP
#include <string>

#include "Engine/Sprite.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Unit.hpp"

class Enemy;
class PlayScene;
class Turret;
namespace Engine {
struct Point;
}  // namespace Engine

class Bullet : public Engine::Sprite {
protected:
	float speed;
	float damage;
	Unit* parent;
	Team  team;
	std::vector<StatusEffect> effectOnEnemy;
	PlayScene* getPlayScene();
	virtual void OnExplode(Enemy* enemy);
	void GiveEffect(Enemy* enemy);
	float explosionRadius = 0;
public:
	Enemy* Target = nullptr;
	explicit Bullet(std::string img, float speed, float damage, Engine::Point position, Engine::Point forwardDirection, float rotation, Unit* parent);
	explicit Bullet(std::string img, float speed, float damage, Engine::Point position, Engine::Point forwardDirection, float rotation, Unit* parent, std::vector<StatusEffect> effects);
	void Update(float deltaTime) override;
};

#endif // BULLET_HPP
