#ifndef FINALPROJECT_BASE_HPP
#define FINALPROJECT_BASE_HPP
#include "Building/Building.hpp"
#include "Turret.hpp"

class Base :public Building{
public:
    static const int Price;
    Base(float x, float y,Team team);
    void Kill() override;
};


#endif //FINALPROJECT_BASE_HPP
