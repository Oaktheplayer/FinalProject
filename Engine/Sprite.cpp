#include <allegro5/allegro.h>
#include <memory>

#include "UI/Component/Image.hpp"
#include "Engine/Point.hpp"
#include "Sprite.hpp"

#include <iostream>

namespace Engine {
	Sprite::Sprite(std::string img, float x, float y, float w, float h, float anchorX, float anchorY,
		float rotation, float vx, float vy, unsigned char r, unsigned char g, unsigned char b, unsigned char a) :
		Image(img, x, y, w, h, anchorX, anchorY), Rotation(rotation), Velocity(Point(vx, vy)), Tint(al_map_rgba(r, g, b, a)) {
	}
	void Sprite::Draw(float scale, float cx, float cy, float sx, float sy) const {
		al_draw_tinted_scaled_rotated_bitmap(bmp.get(), Tint, Anchor.x * GetBitmapWidth(), Anchor.y * GetBitmapHeight(),
			(Position.x-sx)*scale	+ 	cx,
			(Position.y-sy)*scale 	+	cy,
			Size.x / GetBitmapWidth() 	* 	scale,
			Size.y / GetBitmapHeight()	*	scale, Rotation, 0);
	}
    void Sprite::Update(float deltaTime)
    {
		
		std::cerr<<"Update:\tstill good\n";
        Position.x += Velocity.x * deltaTime;
		Position.y += Velocity.y * deltaTime;
    }
};
