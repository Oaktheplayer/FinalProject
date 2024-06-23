#ifndef FINALPROJECT_MAPEDITSCENE_HPP
#define FINALPROJECT_MAPEDITSCENE_HPP

#include <allegro5/allegro_audio.h>
#include <list>
#include <queue>
#include <memory>
#include <utility>
#include <vector>

#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "Turret/Turret.hpp"
#include "Turret/TurretButton.hpp"

class Enemy;
class Turret;
namespace Engine {
    class Group;
    class Image;
    class Label;
    class Sprite;
}

class MapEditScene  : public Engine::IScene {
private:
    ALLEGRO_SAMPLE_ID bgmId;
protected:
    enum TileType {
        TILE_DIRT,
        TILE_FLOOR,
        TILE_WATER
    };
    enum BuildingType {
        None,
        wall,
        Laser,
        Missile,
        FlameThrower,
        MachineGun,
        base
    };
public:

    TileType CurBrushType;
    int MapWidth, MapHeight;
    int buildingExist;
    int brushUsed;
    int baseCnt;
    static const int BlockSize=64;
    float scale;
    Engine::Point center;
    Engine::Point sight;
    Engine::Point sight0;
    Engine::Point sight_dir;
    float	sight_speed;
    // For everything on the map (non-UI)
    Group* TileMapGroup;
    Group* BuildingGroup;
    Group* MapComponent;
    Group* GroundEffectGroup;
    Group* UIGroup;
    Engine::Image* imgTarget;
    Building* preview;
    Engine::Sprite* brush;
    Engine::ImageButton* Frame1,* Frame2,* Frame3;
    std::vector<std::vector<TileType>> mapTerrain;
    std::vector<std::vector<Engine::Image*>> mapTerrainPtr;
    std::vector<std::vector<Building*>> mapBuildings;
    explicit MapEditScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Draw(float scale=1, float cx=0, float cy=0, float sx=0, float sy=0) const override;
    void OnMouseScroll(int mx, int my, int delta) override;
    void OnMouseDown(int button, int mx, int my) override;
    void OnMouseMove(int mx, int my) override;
    void OnMouseUp(int button, int mx, int my) override;
    void OnKeyDown(int keyCode) override;
    void OnKeyUp(int keyCode) override;
    void ConstructUI();
    void BtnClicked(int id);
    void BackOnClick(int stage);
    void SaveBtnClicked(int id);
    void BrushClick(int id);
    void ChangeBrush(int id);
    bool CheckSpaceValid(int x, int y);
    void InitializeMap();
};

#endif //FINALPROJECT_MAPEDITSCENE_HPP
