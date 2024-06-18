#include <string>

#include "SoldierEnemy.hpp"
const int SoldierEnemy::Price = 5;
SoldierEnemy::SoldierEnemy(int x, int y, Team team) : Enemy("play/enemy-1.png", x, y,team, 10, 50, 5, 5, 2,Price) {
	// TODO: [CUSTOM-TOOL] You can imitate the 2 files: 'SoldierEnemy.hpp', 'SoldierEnemy.cpp' to create a new enemy.
}
