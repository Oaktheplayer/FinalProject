//
// Created by user on 2024/6/12.
//

#ifndef WALL_HPP
#define WALL_HPP
#include "Turret.hpp"

class wall: public Turret {
public:
    static const int Price;
    wall(float x, float y,Team team);
    void Draw(float scale=1, float cx=0, float cy=0, float sx=0, float sy=0) const override;
    void Update(float deltaTime) override;
};


#endif //FINALPROJECT_WALL_HPP
