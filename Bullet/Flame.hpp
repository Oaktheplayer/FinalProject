#ifndef FLAME_HPP
#define FLAME_HPP
#include "Bullet.hpp"
#include "Engine/Unit.hpp"
#include <vector>

class Enemy;
class Turret;
namespace Engine {
struct Point;
}  // namespace Engine

class Flame : public Bullet {
public:
	explicit Flame(Engine::Point position, Engine::Point forwardDirection, float rotation, Unit* parent, std::vector<StatusEffect> effects);
	void OnExplode(Unit* enemy) override;
};
#endif // FLAME_HPP
