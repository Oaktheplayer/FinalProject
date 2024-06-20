//
// Created by user on 2024/6/12.
//


#ifndef WALL_HPP
#define WALL_HPP
#include "Building/Building.hpp"
#include "Turret.hpp"

class Wall: //public Turret {
public Building{
public:
    static const int Price;
    Wall(float x, float y,Team team);
};


#endif //FINALPROJECT_WALL_HPP
