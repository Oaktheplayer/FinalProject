
#include <allegro5/allegro.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <vector>
#include <queue>
#include <string>
#include <memory>
#include <iostream>
#include "Engine/AudioHelper.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "UI/Component/Label.hpp"
#include "MapEditScene.hpp"
#include "Engine/Resources.hpp"
#include "Turret/LaserTurret.hpp"
#include "Turret/Wall.hpp"
#include "Turret/MachineGunTurret.hpp"
#include "Turret/MissileTurret.hpp"
#include "Turret/Flamethrower.hpp"
#include "Turret/TurretButton.hpp"
void MapEditScene::Initialize() {
    scale 	= 	1;
    center	=	Point(Engine::GameEngine::GetInstance().GetScreenWidth()/2,Engine::GameEngine::GetInstance().GetScreenHeight()/2);
    sight	=	center;
    sight0	=	center;
    sight_dir	=	Point(0,0);
    sight_speed	=	16;
    MapWidth=20;
    MapHeight=13;
    CurBrushType = 0;
    buildingExist=0;
    AddNewObject(MapComponent = new Group());
    MapComponent->AddNewObject(TileMapGroup = new Group());
    MapComponent->AddNewObject(GroundEffectGroup = new Group());
    AddNewControlObject(UIGroup = new Group());
    ConstructUI();
    imgTarget = new Engine::Image("play/target.png", 0, 0);
    imgTarget->Visible = false;
    preview = nullptr;
    AddNewObject(imgTarget);
    InitializeMap();
    bgmId = AudioHelper::PlayBGM("play.ogg");
}
void MapEditScene::Terminate() {
    AudioHelper::StopBGM(bgmId);
    IScene::Terminate();
}
void MapEditScene::Update(float deltaTime) {
    if(sight.x <=0)sight.x=0;
    else if(sight.x>=MapWidth*BlockSize)sight.x=MapWidth*BlockSize;
    if(sight.y <=0)sight.y=0;
    else if(sight.y>=MapHeight*BlockSize)sight.y=MapHeight*BlockSize;
    sight = sight + sight_dir*sight_speed;
    IScene::Update(deltaTime);
    if (preview) {
        preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
        preview->Update(deltaTime);
    }
}
void MapEditScene::Draw(float scale, float cx, float cy, float sx, float sy) const {
    al_clear_to_color(al_map_rgb(0, 0, 0));
    MapComponent->Draw(this->scale, center.x, center.y, sight.x, sight.y);
    UIGroup->Draw();
    if(imgTarget->Visible)	imgTarget->Draw(this->scale, center.x, center.y, sight.x, sight.y);
    if(preview)	{
        preview->Draw(this->scale,preview->Position.x,preview->Position.y,preview->Position.x,preview->Position.y);
    }
}
void MapEditScene::OnMouseScroll(int mx, int my, int delta){
    float	pre_s	=	scale;
    scale+= (float)delta /4;
    if(scale>2)		scale=2;
    else if(scale<0.25)	scale=0.25;

    Point	mouse(mx,my);
    sight	=	(mouse-center)/pre_s + sight - (mouse-center)/scale;
}
void MapEditScene::OnMouseDown(int button, int mx, int my) {
    if ((button & 1) && !imgTarget->Visible && preview) {
        RemoveObject(preview->GetObjectIterator());
        preview = nullptr;
    }
    IScene::OnMouseDown(button, mx, my);
}
void MapEditScene::OnMouseMove(int mx, int my) {
    IScene::OnMouseMove(mx, my);
    const int x = ((float)(mx-center.x)/scale + sight.x)	/BlockSize;
    const int y = ((float)(my-center.y)/scale + sight.y)	/BlockSize;
    if (!preview || x < 0 || x >= MapWidth || y < 0 || y >= MapHeight) {
        imgTarget->Visible = false;
        return;
    }
    imgTarget->Visible = true;
    imgTarget->Position.x = x * BlockSize;
    imgTarget->Position.y = y * BlockSize;
}
void MapEditScene::OnMouseUp(int button, int mx, int my) {
    IScene::OnMouseUp(button, mx, my);
    if (!imgTarget->Visible)
        return;
    const int x = ((float)(mx-center.x)/scale + sight.x) 	/	BlockSize;
    const int y = ((float)(my-center.y)/scale + sight.y)	/	BlockSize;
    if (button & 1) {
        if (x>=0 && x<=MapWidth && y>=0 && y<=MapWidth && !mapBuildings[y][x]) {
            if (!preview || (UIGroup->Visible && mx>=Engine::GameEngine::GetInstance().GetScreenWidth()-320))
                return;
            if (!CheckSpaceValid(x, y ,preview)) {
                Engine::Sprite* sprite;
                GroundEffectGroup->AddNewObject(sprite = new DirtyEffect("play/target-invalid.png", 1, x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2));
                sprite->Rotation = 0;
                return;
            }
            preview->GetObjectIterator()->first = false;
            RemoveObject(preview->GetObjectIterator());
            // Construct real turret.
            preview->Position.x = x * BlockSize + BlockSize / 2;
            preview->Position.y = y * BlockSize + BlockSize / 2;
            preview->Enabled = false;
            preview->Preview = false;
            preview->Tint = al_map_rgba(255, 255, 255, 255);
            MapComponent->AddNewObject(preview);
            mapBuildings[y][x] = preview;
            buildingExist++;
            preview->Update(0);
            preview = nullptr;
            OnMouseMove(mx, my);
        }
    }
}
void MapEditScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    if (keyCode == ALLEGRO_KEY_1) {
        // Hotkey for MachineGunTurret.
        BtnClicked(0);
    }
    else if (keyCode == ALLEGRO_KEY_2) {
        // Hotkey for LaserTurret.
        BtnClicked(1);
    }
    else if (keyCode == ALLEGRO_KEY_3) {
        // Hotkey for MissileTurret.
        BtnClicked(2);
    }
    else if (keyCode == ALLEGRO_KEY_4) {
        // Hotkey for Flamethrower.
        BtnClicked(3);
    }
    else if (keyCode == ALLEGRO_KEY_5) {
        // Hotkey for Wall.
        BtnClicked(4);
    }
    else if (keyCode == ALLEGRO_KEY_W) {
        sight_dir.y=-1;
    }
    else if (keyCode == ALLEGRO_KEY_S) {
        sight_dir.y=1;
    }
    else if (keyCode == ALLEGRO_KEY_A) {
        sight_dir.x=-1;
    }
    else if (keyCode == ALLEGRO_KEY_D) {
        sight_dir.x=1;
    }
    else if(keyCode == ALLEGRO_KEY_X){
        scale=1;
        sight	=	sight0;
    }
}
void MapEditScene::OnKeyUp(int keyCode){
    if(keyCode	==	ALLEGRO_KEY_W || keyCode	==	ALLEGRO_KEY_S){
        sight_dir.y=0;
    }
    if(keyCode	==	ALLEGRO_KEY_A || keyCode	==	ALLEGRO_KEY_D){
        sight_dir.x=0;
    }
}
void MapEditScene::InitializeMap() {
    mapTerrain = std::vector<std::vector<TileType>>(MapHeight, std::vector<TileType>(MapWidth));
    mapTerrainPtr = std::vector<std::vector<Engine::Image*>>(MapHeight, std::vector<Engine::Image*>(MapWidth));
    mapBuildings = std::vector<std::vector<Turret*>>(MapHeight, std::vector<Turret*>(MapWidth));
    for (int i = 0; i < MapHeight; i++) {
        for (int j = 0; j < MapWidth; j++) {
            mapTerrain[i][j]=TILE_DIRT;
            TileMapGroup->AddNewObject(mapTerrainPtr[i][j]=new Engine::Image("play/dirt.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
        }
    }
}
void MapEditScene::ConstructUI() {
    // Background
    UIGroup->AddNewObject(new Engine::Image("play/sand.png", 1280, 0, 320, 832));

    TurretButton* btn;
    int y = 176;
    int x = 1294;
    int dx = 1370-1294;
    int i = 0;
    int j = 0;
    // Button 0
    btn = new TurretButton("play/button1.png", "play/button2.png",
                           Engine::Sprite("play/tower-base.png", x+i%4*dx, y, 0, 0, 0, 0),
                           Engine::Sprite("play/turret-1.png", x+i%4*dx, y - 8, 0, 0, 0, 0)
            , x+i%4*dx, y, 0);
    btn->SetOnClickCallback(std::bind(&MapEditScene::BtnClicked, this, 0));
    UIGroup->AddNewControlObject(btn);
    i++;
    // Button 1
    btn = new TurretButton("play/button1.png", "play/button2.png",
                           Engine::Sprite("play/tower-base.png", x+i%4*dx, y, 0, 0, 0, 0),
                           Engine::Sprite("play/turret-2.png", x+i%4*dx, y - 8, 0, 0, 0, 0)
            , x+i%4*dx, y, 0);
    btn->SetOnClickCallback(std::bind(&MapEditScene::BtnClicked, this, 1));
    UIGroup->AddNewControlObject(btn);
    i++;
    // Button 2
    btn = new TurretButton("play/button1.png", "play/button2.png",
                           Engine::Sprite("play/tower-base.png", 1446, y, 0, 0, 0, 0),
                           Engine::Sprite("play/turret-3.png", 1446, y - 8, 0, 0, 0, 0)
            , x+i%4*dx, y, 0);
    btn->SetOnClickCallback(std::bind(&MapEditScene::BtnClicked, this, 2));
    UIGroup->AddNewControlObject(btn);
    i++;
    // Button 3
    btn = new TurretButton("play/button1.png", "play/button2.png",
                           Engine::Sprite("play/tower-base.png", x+i%4*dx, y, 0, 0, 0, 0),
                           Engine::Sprite("play/turret-6.png", x+i%4*dx, y - 8, 0, 0, 0, 0)
            , x+i%4*dx, y, 0);
    btn->SetOnClickCallback(std::bind(&MapEditScene::BtnClicked, this, 3));
    UIGroup->AddNewControlObject(btn);
    i++;
    // Button 4
    btn = new TurretButton("play/button1.png", "play/button2.png",
                           Engine::Sprite("play/tower-base.png", x+i%4*dx, y+(i/4)*dx, 0, 0, 0, 0),
                           Engine::Sprite("play/tower-base.png", x+i%4*dx, y+(i/4)*dx, 0, 0, 0, 0)
            , x+i%4*dx, y+(i/4)*dx, 0);
    btn->SetOnClickCallback(std::bind(&MapEditScene::BtnClicked, this, 4));
    UIGroup->AddNewControlObject(btn);
    i++;

    Engine::ImageButton* Btn;
    Btn = new Engine::ImageButton("play/dirt.png", "play/dirt.png", x+j%4*(dx+30), y+(j/4)*dx+300 , 75, 75);
    Btn->SetOnClickCallback(std::bind(&MapEditScene::ChangeBrush, this, j));
    UIGroup->AddNewControlObject(Btn);
    j++;

    Btn = new Engine::ImageButton("play/grass.png", "play/grass.png", x+j%4*(dx+30), y+(j/4)*dx+300 , 75, 75);
    Btn->SetOnClickCallback(std::bind(&MapEditScene::ChangeBrush, this, j));
    UIGroup->AddNewControlObject(Btn);
    j++;

    Btn = new Engine::ImageButton("play/water.png", "play/water.png", x+j%4*(dx+30), y+(j/4)*dx+300 , 75, 75);
    Btn->SetOnClickCallback(std::bind(&MapEditScene::ChangeBrush, this, j));
    UIGroup->AddNewControlObject(Btn);

    //save button
    Btn = new Engine::ImageButton("stage-select/button2.png", "stage-select/button1.png",x, y+550, 290, 60);
    Btn->SetOnClickCallback(std::bind(&MapEditScene::SaveBtnClicked, this, 0));
    UIGroup->AddNewControlObject(Btn);
    UIGroup->AddNewObject(new Engine::Label("Save", "pirulen.ttf", 48, x+145, y+580, 0, 0, 0, 255, 0.5, 0.5));
}
void MapEditScene::BtnClicked(int id) {
    std::cerr<<"button clicked\n";
    if (preview){
        RemoveObject(preview->GetObjectIterator());
    }
    if (id == 0)
        preview = new MachineGunTurret(0, 0,RED);
    else if (id == 1 )
        preview = new LaserTurret(0, 0,RED);
    else if (id == 2 )
        preview = new MissileTurret(0, 0,RED);
    else if (id == 3 )
        preview = new Flamethrower(0, 0,RED);
    else if (id == 4 )
        preview = new wall(0, 0,RED);
    else preview=nullptr;
    if (!preview){
        std::cerr<<"Button click fail\n";
        imgTarget->Visible=false;
        return;
    }
    std::cerr<<"Button clicking\n";
    preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
    preview->Tint = al_map_rgba(255, 255, 255, 200);
    preview->Enabled = false;
    preview->Preview = true;
    AddNewObject(preview);
    OnMouseMove(Engine::GameEngine::GetInstance().GetMousePosition().x, Engine::GameEngine::GetInstance().GetMousePosition().y);
    std::cerr<<"Button click end\n";
}

void MapEditScene::ChangeBrush(int id){
    if(id!=CurBrushType)CurBrushType=id;
}

void MapEditScene::SaveBtnClicked(int id){
    std::cerr<<"start saving\n";
    std::ofstream data("Resource/map4.txt");
    if(data.is_open()){
        data<<MapWidth<<" "<<MapHeight;
        if(buildingExist)data<<" 1"<<std::endl;
        else data<<" 0"<<std::endl;
        for(int i=0;i<MapHeight;i++){
            for(int j=0;j<MapWidth;j++){
                data<<mapTerrain[i][j];
            }
            data<<std::endl;
        }
        data<<std::endl;
        std::cerr<<"done saving terrain\n";
        if(buildingExist) {
            for (int i = 0; i < MapHeight; i++) {
                for (int j = 0; j < MapWidth; j++) {
                    if (dynamic_cast<MachineGunTurret *> (mapBuildings[i][j]))data << MachineGun;
                    else if (dynamic_cast<LaserTurret *> (mapBuildings[i][j]))data << Laser;
                    else if (dynamic_cast<wall *> (mapBuildings[i][j]))data << Wall;
                    else if (dynamic_cast<Flamethrower *> (mapBuildings[i][j]))data << FlameThrower;
                    else if (dynamic_cast<MissileTurret *> (mapBuildings[i][j]))data << Missile;
                    else data << None;
                }
                data << std::endl;
            }
            std::cerr<<"done saving Building\n";
        }
        data.close();
    }
}

bool MapEditScene::CheckSpaceValid(int x, int y,Turret *turret) {
    if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)return false;
    if(mapBuildings[y][x])return false;
    return true;
}