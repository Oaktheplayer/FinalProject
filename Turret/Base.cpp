#include "Base.hpp"
const int Base::Price = 10;
const int HitPoint = 1000;
Base::Base(float x, float y,Team team) :
        Building("play/base.png", x, y,team, PlayScene::BlockSize/2, HitPoint,Price) {
}

void Base::Kill() {
    Unit::Kill();
    getPlayScene()->BaseExsist= false;
}