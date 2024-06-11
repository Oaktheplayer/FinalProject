#ifndef LASERBULLET_HPP
#define LASERBULLET_HPP
#include "Bullet.hpp"
#include "Engine/Unit.hpp"

class Enemy;
class Turret;
namespace Engine {
struct Point;
}  // namespace Engine

class LaserBullet : public Bullet {
public:
	explicit LaserBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Unit* parent);
	void OnExplode(Unit* enemy) override;
};
#endif // LASERBULLET_HPP
