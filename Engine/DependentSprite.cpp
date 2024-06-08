#include <allegro5/allegro.h>
#include <memory>

#include "UI/Component/Image.hpp"
#include "Engine/Point.hpp"
#include "DependentSprite.hpp"


namespace Engine {
	DependentSprite::DependentSprite(std::string img, float x, float y, float w, float h, float anchorX, float anchorY,
		float rotation, float vx, float vy, unsigned char r, unsigned char g, unsigned char b, unsigned char a) :
		Sprite(img,x,y,w,h,anchorX,anchorY,rotation,vx,vy,r,g,b,a),center(),sight(){
	}
	void DependentSprite::Draw() const {
		al_draw_tinted_scaled_rotated_bitmap(bmp.get(), Tint, Anchor.x * GetBitmapWidth(), Anchor.y * GetBitmapHeight(),
			(Position.x - sight.x)*scale+center.x, (Position.y-sight.y)*scale+center.y, Size.x / GetBitmapWidth()*scale, Size.y / GetBitmapHeight()*scale, Rotation, 0);
	}
};
