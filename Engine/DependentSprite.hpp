#ifndef DEPENDENTSPRITE_HPP
#define DEPENDENTSPRITE_HPP
#include <allegro5/color.h>
#include <string>

#include "UI/Component/Image.hpp"
#include "Sprite.hpp"
#include "Engine/Point.hpp"
using namespace Engine;

extern float scale;

namespace Engine {
	/// <summary>
	/// Image that supports rotation, velocity, tint, and collision radius.
	/// </summary>
	class DependentSprite : public Sprite {
	public:
		Point 	center;
		Point 	sight;
		//int 	scale;
		DependentSprite(std::string img, float x, float y, float w = 0, float h = 0, float anchorX = 0.5f, float anchorY = 0.5f,
			float rotation = 0, float vx = 0, float vy = 0, unsigned char r = 255, unsigned char g = 255, unsigned char b = 255, unsigned char a = 255);

		void Draw() const override;
	};
}
#endif // DEPENDENTSPRITE_HPP

