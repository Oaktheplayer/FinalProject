#include <allegro5/color.h>
#include <allegro5/allegro_primitives.h>
#include "Wall.hpp"
const int wall::Price = 10;
wall::wall(float x, float y,Team team) :
    Turret("play/tower-base.png", "play/tower-base.png", x, y,team, 0, Price, 0.5, 0) {
    // Move center downward, since we the turret head is slightly biased upward.
    Anchor.y += 8.0f / GetBitmapHeight();
    hp = 500;
}
void wall::Update(float deltaTime) {
    Unit::Update(deltaTime);
    PlayScene *scene = getPlayScene();
    imgBase.Position = Position;
    imgBase.Tint = Tint;
}
void wall::Draw(float scale, float cx, float cy, float sx, float sy) const {
    if (Preview) {
        al_draw_filled_circle(
                (Position.x-sx)*scale	+ 	cx,
                (Position.y-sy)*scale 	+	cy,
                range*scale,
                al_map_rgba(0, 255, 0, 50));
    }
    imgBase.Draw(scale, cx, cy, sx, sy);
	//Unit::Draw(scale, cx, cy, sx, sy);
    if (PlayScene::DebugMode) {
        // Draw target radius.
        al_draw_circle((Position.x-sx)*scale + cx, (Position.y-sy)*scale + cy, range*scale, al_map_rgb(0, 0, 255), 2);
        // Draw collision radius.
		al_draw_circle(
			(Position.x-sx)*scale	+ 	cx,
			(Position.y-sy)*scale 	+	cy,
			CollisionRadius*scale,
			al_map_rgb(255, 0, 0), 2);
		//Display health
		std::string text = std::to_string((int)hp);
		al_draw_text(font.get(),al_map_rgba(255, 255, 255, 127),
			(Position.x-sx)*scale	+ 	cx,
			(Position.y-sy)*scale 	+	cy,
			0,text.c_str());
    }
}