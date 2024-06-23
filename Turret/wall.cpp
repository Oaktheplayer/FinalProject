#include "Wall.hpp"
const int Wall::Price = 10;
const int HitPoint = 400;
Wall::Wall(float x, float y,Team team) :
    Building("play/wall.png", x, y,team, PlayScene::BlockSize/2, HitPoint,Price) {
    // Move center downward, since we the turret head is slightly biased upward.
}