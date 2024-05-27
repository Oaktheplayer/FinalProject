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
    currentLength   =   10;

    Engine::ImageButton*    btn;
    Engine::Label*          title;

    //data[10];
    ReadScoreboard();
    
    AddNewObject(new Engine::Label("SCOREBOARD", "pirulen.ttf", 48, halfW, halfH / 4, 255, 255, 255, 255, 0.5, 0.5));
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
    btn->SetOnClickCallback(std::bind(&ScoreboardScene::ToPage, this, false));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Prev Page", "pirulen.ttf", 48, halfW/2, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", 3*halfW/2 - 200, halfH * 3 / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&ScoreboardScene::ToPage, this, true));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("NEXT PAGE", "pirulen.ttf", 48, halfW*3/2, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));
    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
	bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);

    LoadScoreBoard(0);
}



void ScoreboardScene::Terminate() {
	AudioHelper::StopSample(bgmInstance);
	bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
	IScene::Terminate();
    //ClearScoreBoard();
    record.clear();
}
void ScoreboardScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}

void ScoreboardScene::ReadScoreboard(){
    std::string     filename    =   "../PlayerData/Scoreboards/scoreboard.txt";
    std::ifstream   fin(filename);
    std::string n;
    std::string s;
    std::string d;
    while(fin>>n){
        fin>>s;
        fin>>d;
        record.push_back(Record(n,s,d));
    } 
    fprintf(stderr,"%d records in total\n",record.size());
    maxPage =   record.size()/10 + 1;
}

void ScoreboardScene::ClearScoreBoard(){
    for(int i=0;i<currentLength;i++){
        if(usernames[i])    RemoveObject(usernames[i]->GetObjectIterator());
        if(scores[i])       RemoveObject(scores[i]->GetObjectIterator());
        if(dates[i])       RemoveObject(dates[i]->GetObjectIterator());
    }
}

void ScoreboardScene::LoadScoreBoard(int page){
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int start   =   page*10;
    currentLength   =   0;
    for(int i=0;i<10;i++){
        //DONE: it won't fucking work (it works now, you need to remove thing.getObjectIterator())
        if(start+i>=record.size()) break;
            currentLength++;
        
        usernames[i]    =   new Engine::Label(record[i+start].username, "pirulen.ttf", 32, w/4, h / 8 + 64 + 32*i, 255, 255, 255, 255, 0.5, 0.5);
        AddNewObject(usernames[i]);
        scores[i]       =   new Engine::Label(record[i+start].score, "pirulen.ttf", 32, 2*w/4, h / 8 + 64 + 32*i, 255, 255, 255, 255, 0.5, 0.5);
        AddNewObject(scores[i]);
        dates[i]       =   new Engine::Label(record[i+start].date, "pirulen.ttf", 32, 3*w/4, h / 8 + 64 + 32*i, 255, 255, 255, 255, 0.5, 0.5);
        AddNewObject(dates[i]);
    }
    fprintf(stderr,"current length: %d)\n",currentLength);
    //AddNewObject(usernames)
}

void ScoreboardScene::ToPage(bool isToNext){
    int delta   =   isToNext?   1:-1;
    if((page+delta)*10 > record.size() || page+delta<0){
        fprintf(stderr,"no neet page (currently on %d)\n",page);
        return;
    }
    page    +=  delta;
    fprintf(stderr,"change page to %d\n",page);
    // for(int i=0;i<currentLength*2;i++){
    //     RemoveObject(prev(objects.end()));
    // }
    ClearScoreBoard();
    LoadScoreBoard(page);
}

Record::Record(std::string  n,  std::string s,  std::string d):     username(n),score(s),date(d) {}

Record::Record(std::string  n, std::string  s) : username(n), score(s), date("N/A") {};
// std::istream &operator>>(std::istream    is, Record  r){
//     std::string s;
//     int n;
//     is>>s>>n;
//     r.score =   n;  r.username  =   s;
//     return  is;
// };
