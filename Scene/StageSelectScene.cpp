#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "PlayScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "UI/Component/Slider.hpp"
#include "StageSelectScene.hpp"
#include <vector>

#include <iostream>

void StageSelectScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    halfW = w / 2;
    halfH = h / 2;
    stageId =   1;
    h1    =     halfH* 1/4;
	scrollY =   0;
    AddNewControlObject(LevelButtons    =   new Group());
    Engine::ImageButton* btn;
    btn = new Engine::ImageButton("stage-select/button2.png", "stage-select/button1.png", halfW*1/3 - halfBW, halfH * 7 / 4 - deltah -halfBH, buttonWidth, buttonHeight);
    btn->SetOnClickCallback(std::bind(&StageSelectScene::BackOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW*1/3, halfH * 7 / 4 - deltah, 0, 0, 0, 255, 0.5, 0.5));

    LoadLevelButtons();

    btn = new Engine::ImageButton("stage-select/button2.png", "stage-select/button1.png", halfW*5/3 - halfBW, halfH * 7 / 4 - deltah -halfBH, buttonWidth, buttonHeight);
    btn->SetOnClickCallback(std::bind(&StageSelectScene::ScoreboardOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Scoreboard", "pirulen.ttf", 40, halfW*5/3, halfH * 7 / 4 - deltah, 0, 0, 0, 255, 0.5, 0.5));



    //Left Button
    btn = new Engine::ImageButton("stage-select/button2.png", "stage-select/button1.png", halfW*1/3 - halfBW, halfH * 7/4 - halfBH, buttonWidth, buttonHeight);
    btn->SetOnClickCallback(std::bind(&StageSelectScene::ToStage, this, false));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Prev Page", "pirulen.ttf", 48, halfW*1/3, halfH * 7/4 , 0, 0, 0, 255, 0.5, 0.5));
    
    //Right Button
    btn = new Engine::ImageButton("stage-select/button2.png", "stage-select/button1.png", halfW*5/3 - halfBW, halfH * 7/4 - halfBH, buttonWidth, buttonHeight);
    btn->SetOnClickCallback(std::bind(&StageSelectScene::ToStage, this, true));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("NEXT PAGE", "pirulen.ttf", 48, halfW*5/3, halfH * 7/4, 0, 0, 0, 255, 0.5, 0.5));
    
    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
	bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);

    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
	bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
}

void StageSelectScene::Terminate() {
	AudioHelper::StopSample(bgmInstance);
	bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
	IScene::Terminate();
}
void StageSelectScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("start");
}
void StageSelectScene::PlayOnClick(int level) {
    PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play"));
    scene->StageId  =   stageId-1;
    scene->MapId = level;
    Engine::GameEngine::GetInstance().ChangeScene("play");
}
void StageSelectScene::ScoreboardOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("scoreboard");
}

void StageSelectScene::LoadLevelButtons(){
    scrollY =   0;
    int h0=h1;//h0 = halfH/2 - buttonHeight;
    //int h1 = halfH* 1/4,    h2 = halfH* 3/4;
    
    
    for(int i=0;i<levelCnt;i++){
        Engine::ImageButton* btn = new Engine::ImageButton("stage-select/button2.png", "stage-select/button1.png", halfW - halfBW, h0 -halfBH + deltah*i, buttonWidth, buttonHeight);
        btn->SetOnClickCallback(std::bind(&StageSelectScene::PlayOnClick, this, i+1));
        LevelButtons->AddNewControlObject(btn);
        LevelButtons->AddNewObject(new Engine::Label(std::to_string(stageId)+"-"+std::to_string(i+1), "pirulen.ttf", 48, halfW, h0 +deltah*i, 0, 0, 0, 255, 0.5, 0.5));
    }
    h2  =   levelCnt*deltah - 800;
}

void StageSelectScene::ToStage(bool isToNext){
    if(isToNext){
        if(stageId<max_stage_count) stageId++;
        else                        return;
    }else
    {
        if(stageId>1)               stageId--;
        else                        return;
    }
    
    for(auto i: LevelButtons->GetObjects()){
        RemoveObject(i->GetObjectIterator());
    }
    //RemoveObject(LevelButtons->GetObjectIterator());
    LoadLevelButtons();
}

void StageSelectScene::OnMouseScroll(int mx, int my, int delta){
    int dh = delta*buttonHeight*1/2;
    if(scrollY-dh<0 || scrollY-dh>h2)return;
    else scrollY-=dh;
    for(auto i:LevelButtons->GetObjects()){
        i->Position.y+= dh;
    }
}
