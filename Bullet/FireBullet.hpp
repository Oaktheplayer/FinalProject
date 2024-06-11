#ifndef FIREBULLET_HPP
#define FIREBULLET_HPP
#include "Bullet.hpp"
#include "Engine/Unit.hpp"

class Enemy;
class Turret;
namespace Engine {
struct Point;
}  // namespace Engine

class FireBullet : public Bullet {
public:
	explicit FireBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Unit* parent);
	void OnExplode(Unit* enemy) override;
};
#endif // FIREBULLET_HPP
