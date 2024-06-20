//
// Created by user on 2024/6/12.
//

#ifndef BUILDING_HPP
#define BUILDING_HPP
#include "Engine/Unit.hpp"
#include "Scene/PlayScene.hpp"

class PlayScene;
class Building: public Unit {
protected:
    int price;
public:
    Building(std::string img, float x, float y,Team team, float radius, float hp, int price);
    void Draw(float scale=1, float cx=0, float cy=0, float sx=0, float sy=0) const override;
    void Update(float deltaTime) override;
    void Kill() override;
};


#endif //FINALPROJECT_WALL_HPP
