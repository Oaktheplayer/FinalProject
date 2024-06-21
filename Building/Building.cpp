#include <allegro5/color.h>
#include <allegro5/allegro_primitives.h>
#include "Building.hpp"
Building::Building(std::string img, float x, float y,Team team, float radius, float hp, int price) :
    Unit(img, x, y,team, radius, hp, price) {
    hp = 500;
}
void Building::Update(float deltaTime) {
    Unit::Update(deltaTime);
}
void Building::Draw(float scale, float cx, float cy, float sx, float sy) const {
	if (Preview) {
		al_draw_filled_circle(
			(Position.x-sx)*scale	+ 	cx,
			(Position.y-sy)*scale 	+	cy,
			range*scale,
			al_map_rgba(0, 255, 0, 50));
	}
	Unit::Draw(scale, cx, cy, sx, sy);
	if (PlayScene::DebugMode) {
		// Draw target radius.
		al_draw_circle((Position.x-sx)*scale + cx, (Position.y-sy)*scale + cy, range*scale, al_map_rgb(0, 0, 255), 2);
	}
}

// int Building::GetPrice() const {
//     return price;
// }

// int Building::GetHp() const{
//     return (int)hp;
// }

void Building::Kill()
{
    getPlayScene()->RemoveBuilding(Position.x/PlayScene::BlockSize,Position.y/PlayScene::BlockSize);
	Unit::Kill();
}