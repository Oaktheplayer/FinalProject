#ifndef FINALPROJECT_ENEMYBUTTON_H
#define FINALPROJECT_ENEMYBUTTON_H
#include <string>

#include "UI/Component/ImageButton.hpp"
#include "Engine/Sprite.hpp"

class PlayScene;

class EnemyButton : public Engine::ImageButton {
protected:
    PlayScene* getPlayScene();
public:
    int money;
    Engine::Sprite Enemy;
    EnemyButton(std::string img, std::string imgIn, Engine::Sprite Enemy, float x, float y, int money);
    void Update(float deltaTime) override;
    void Draw(float scale=1, float cx=0, float cy=0, float sx=0, float sy=0) const override;
};
#endif //FINALPROJECT_ENEMYBUTTON_H
