#include <allegro5/color.h>

#include "Engine/GameEngine.hpp"
#include "Engine/IScene.hpp"
#include "Scene/PlayScene.hpp"
#include "EnemyButton.h"
PlayScene* EnemyButton::getPlayScene() {
    return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}
EnemyButton::EnemyButton(std::string img, std::string imgIn, Engine::Sprite Enemy, float x, float y, int money) :
        ImageButton(img, imgIn, x, y), money(money), Enemy(Enemy) {
}
void EnemyButton::Update(float deltaTime) {
    ImageButton::Update(deltaTime);
    if (getPlayScene()->GetMoney() >= money) {
        Enabled = true;
    } else {
        Enabled = false;
    }
}
void EnemyButton::Draw(float scale, float cx, float cy, float sx, float sy) const {
    ImageButton::Draw();
    Enemy.Draw();
}