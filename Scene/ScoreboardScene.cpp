#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>
#include <fstream>
#include <vector>
#include <list>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "PlayScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "UI/Component/Slider.hpp"
#include "ScoreboardScene.hpp"


void ScoreboardScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    page    =   0;

    Engine::ImageButton*    btn;
    Engine::Label*          title;

    //data[10];
    ReadScoreboard();
    
    AddNewObject(new Engine::Label("SCOREBOARD", "pirulen.ttf", 48, halfW, halfH / 4, 255, 255, 255, 255, 0.5, 0.5));
    LoadScoreBoard(0);
    // for(int i=0;i<10;i++){
    //     usernames[i] =   new Engine::Label(record[i].username, "pirulen.ttf", 32, halfW/2, halfH / 4 + 64 + 32*i, 255, 255, 255, 255, 0.5, 0.5);
    //     scores[i] =   new Engine::Label(std::to_string(record[i].score), "pirulen.ttf", 32, 3*halfW/2, halfH / 4 + 64 + 32*i, 255, 255, 255, 255, 0.5, 0.5);
    //     AddNewObject(usernames[i]);
    //     AddNewObject(scores[i]);
    // }
    

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH * 3 / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&ScoreboardScene::BackOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW/2 - 200, halfH * 3 / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&ScoreboardScene::ToPage, this, ++page));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Prev Page", "pirulen.ttf", 48, halfW/2, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", 3*halfW/2 - 200, halfH * 3 / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&ScoreboardScene::ToPage, this, --page));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("NEXT PAGE", "pirulen.ttf", 48, halfW*3/2, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));
    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
	bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
}



void ScoreboardScene::Terminate() {
	AudioHelper::StopSample(bgmInstance);
	bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
	IScene::Terminate();
}
void ScoreboardScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}

void ScoreboardScene::ReadScoreboard(){
    std::string     filename    =   "Resource/scoreboard.txt";
    std::ifstream   fin(filename);
    std::string s;
    int n;
    Record  r;
    while(fin>>s){
        fin>>n;
        record.push_back(Record(s,n));
    } 
    maxPage =   record.size()/10 + 1;
}

void ScoreboardScene::LoadScoreBoard(int page){
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int start   =   page*10;
    for(int i=0;i<10;i++){
        //TODO: it won't fucking work
        if(scores[i])   RemoveObject(scores[i]);
        if(page*10+i>record.size()) break;
        AddNewObject(new Engine::Label(record[i+start].username, "pirulen.ttf", 32, w/4, h / 8 + 64 + 32*i, 255, 255, 255, 255, 0.5, 0.5););
        auto& usernames[i]    =   prev(objects.end());
        scores[i]       =   new Engine::Label(std::to_string(record[i+start].score), "pirulen.ttf", 32, 3*w/4, h / 8 + 64 + 32*i, 255, 255, 255, 255, 0.5, 0.5);
        AddNewObject(scores[i]);
    }
    AddNewObject(usernames)
}

void ScoreboardScene::ToPage(int page){
    fprintf(stderr,"change page to %d",page);
    if((page)*10 > record.size() || page<=0) return;
    LoadScoreBoard(page);
}

Record::Record(std::string s, int n):username(s),score(n){};
// std::istream &operator>>(std::istream    is, Record  r){
//     std::string s;
//     int n;
//     is>>s>>n;
//     r.score =   n;  r.username  =   s;
//     return  is;
// };
