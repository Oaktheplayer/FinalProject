#include <functional>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "PlayScene.hpp"
#include "Engine/Point.hpp"
#include "WinScene.hpp"

#include	<fstream>
#include 	<sstream>
#include	<iostream>

void WinScene::Initialize() {
	ticks = 0;
	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int halfW = w / 2;
	int halfH = h / 2;
	username	=	"XDDCC";

	AddNewObject(new Engine::Image("win/benjamin-sad.png", halfW, halfH, 0, 0, 0.5, 0.5));
	AddNewObject(new Engine::Label("You Win!", "pirulen.ttf", 48, halfW, halfH / 4 -10, 255, 255, 255, 255, 0.5, 0.5));
	ReadCurrentData();
	AddNewObject(new Engine::Label("score:" + std::to_string(score), "pirulen.ttf", 36, halfW, halfH / 4 + 48, 255, 255, 255, 255, 0.5, 0.5));
	RecordScore();

	Engine::ImageButton* btn;
	btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 200, halfH * 7 / 4 - 50, 400, 100);
	btn->SetOnClickCallback(std::bind(&WinScene::BackOnClick, this, 2));
	AddNewControlObject(btn);
	AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));
	bgmId = AudioHelper::PlayAudio("win.wav");
}
void WinScene::Terminate() {
	IScene::Terminate();
	AudioHelper::StopBGM(bgmId);
}
void WinScene::Update(float deltaTime) {
	ticks += deltaTime;
	if (ticks > 4 && ticks < 100 &&
		dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play"))->MapId == 2) {
		ticks = 100;
		bgmId = AudioHelper::PlayBGM("happy.ogg");
	}
}
void WinScene::BackOnClick(int stage) {
	// Change to select scene.
	Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}

void WinScene::ReadCurrentData(){
    std::ifstream   fin("Resource/currentdata.txt");
    fin>>score;
}

void WinScene::RecordScore(){
	//TODO: scoreboard isn't saved
	//TODO: custom name
	// std::ifstream	scrbdin("Resource/scoreboard.txt",std::ifstream::in);
	
	// std::ostringstream	text;
	// text	<<	scrbdin.rdbuf();
	// text 	<<	username<<' '<<score<<'\n';
	// std::string output = text.str();
	// std::cout<<output;
	// scrbdin.close();

	// std::ofstream	scrbdout("Resource/scoreboard.txt",std::ofstream::out);
	// scrbdout<<output;
	// scrbdout.close();


	FILE*scrbdf;
	scrbdf=fopen("Resource/scoreboard.txt","a");
	if(scrbdf	==	NULL){
		perror("Error opening file for writing");
        return;
	}
	fprintf(scrbdf,"xddcc %d\n",score);
	if(fflush(scrbdf)!=0){
		perror("Error flushing file buffer");
        fclose(scrbdf);  // Attempt to close the file even if flushing fails
        return;
	}
	if(fclose(scrbdf)!=0){
		perror("Error closing file");
        return;
	}
	perror("No porblem occrured");
}
