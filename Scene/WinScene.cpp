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
#include	<ctime>
#define		CUR	'_'


void WinScene::Initialize()
{
    ticks = 0;
	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int halfW = w / 2;
	int halfH = h / 2;
	username	=	"";		//username	  =	  "XDDCC";
	nameInput	=	"";
	cursor	=	0;
	time_t	rawtime=time(0);
	current_time	=	localtime(&rawtime);

	AddNewObject(new Engine::Image("win/benjamin-sad.png", halfW, halfH + 16, 0, 0, 0.5, 0.5));
	AddNewObject(new Engine::Label("You Win!", "pirulen.ttf", 48, halfW, halfH / 4 - 48, 255, 255, 255, 255, 0.5, 0.5));
	ReadCurrentData();
	AddNewObject(new Engine::Label("score:" + std::to_string(score), "pirulen.ttf", 36, halfW, halfH / 4 , 255, 255, 255, 255, 0.5, 0.5));
	

	Engine::ImageButton* btn;
	btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 200, halfH * 7 / 4 - 50, 400, 100);
	btn->SetOnClickCallback(std::bind(&WinScene::BackOnClick, this, 2));
	AddNewControlObject(btn);
	AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));

	btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 200, halfH/ 2 -60 , 400, 60);
	btn->SetOnClickCallback(std::bind(&WinScene::ChangeName, this));
	AddNewControlObject(btn);
	AddNewObject(nameLabel	= new Engine::Label("name:", "pirulen.ttf", 32, halfW, halfH/ 2 -30, 0, 0, 0, 255, 0.5, 0.5));
	
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
	if(username!="")
		RecordScore();
	Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}

void WinScene::ReadCurrentData(){
    std::ifstream   fin("Resource/currentdata.txt");
    fin>>score;
}

void WinScene::RecordScore(){
	//DONE: scoreboard isn't saved
	//DONE: sort score
	//DONE: custom name
	std::string filename 	=	"../PlayerData/Scoreboards/scoreboard.txt";

	std::ifstream	scrbdin(filename,std::ifstream::in);
	std::stringstream	ssin;
	std::ostringstream	ssout;

	ssin	<<	scrbdin.rdbuf();

	std::string n, d;
	int			scr;
	bool	recorded = false;
	while(ssin>>n){
		ssin>>scr;
		ssin>>d;
		if(scr<score && !recorded){
			ssout	<<	username<<	' ';
			ssout	<<	score<< ' ';
			if(current_time->tm_hour<10)	ssout<<0;
			ssout	<<	current_time->tm_hour	<<	':';
			if(current_time->tm_min<10)		ssout<<0;
			ssout	<<	current_time->tm_min	<<	'_';
			ssout	<<	to_month[current_time->tm_mon];
			if(current_time->tm_mday<10) ssout<<0;
			ssout	<<current_time->tm_mday;
			ssout	<<'_'<<	current_time->tm_year + 1900;
			ssout	<<	std::endl;
			recorded	=	true;
		}
		ssout	<<n<<' '<<scr<<' '<<d<<std::endl;
	}
	if(!recorded){
		ssout	<<	username<<	' ';
		ssout	<<	score<< ' ';

		if(current_time->tm_hour<10)	ssout<<0;
		ssout	<<	current_time->tm_hour	<<	':';
		if(current_time->tm_min<10)		ssout<<0;
		ssout	<<	current_time->tm_min	<<	'_';
		ssout	<<	to_month[current_time->tm_mon];
		if(current_time->tm_mday<10) ssout<<0;
		ssout	<<current_time->tm_mday;
		ssout	<<'_'<<	current_time->tm_year + 1900;
		ssout	<<	std::endl;
	}

	std::string output = ssout.str();
	std::cout<<output;
	scrbdin.close();

	std::ofstream	scrbdout(filename,std::ofstream::out);
	scrbdout<<output;
	scrbdout.close();
}


void WinScene::ChangeName(){
	std::cerr<<"start name insert\n";
	recordKey	=	true;
	cursor		=	nameInput.length();
	nameInput   +=  CUR;
	PutName();
}

void WinScene::OnKeyDown(int keyCode){

	if(keyCode	==	ALLEGRO_KEY_ESCAPE){
		recordKey	=	false;
	}	else
	if(recordKey){
		switch (keyCode){
			case 	ALLEGRO_KEY_ENTER:
				username = nameInput	=	nameInput.substr(0,cursor)+nameInput.substr(cursor+1);
				std::cerr<<"Name saved as: "<<username<<std::endl;
				recordKey	=	false;
				break;
			case	ALLEGRO_KEY_LEFT:
				if(cursor>0){
					nameInput[cursor]	=	nameInput[cursor-1];
					nameInput[cursor-1]	=	CUR;	
					cursor--;
				}
				break;
			case	ALLEGRO_KEY_RIGHT:
				if(cursor+1<nameInput.length()){
					nameInput[cursor]	=	nameInput[cursor+1];
					nameInput[cursor+1]	=	CUR;	
					cursor++;
				}
				break;
			case	ALLEGRO_KEY_BACKSPACE:
				if(cursor>0){
					std::string s = nameInput.substr(cursor);
					nameInput	=	nameInput.substr(0,cursor-1)	+	s;
					cursor--;
				}				
				break;
			case	ALLEGRO_KEY_DELETE:
				if(cursor<nameInput.length()-1){
					std::string s = nameInput.substr(cursor+2);
					nameInput	=	nameInput.substr(0,cursor+1)	+	s;
				}
				break;
			default:
				if(	nameInput.length()<=12){
					if(	(keyCode>=ALLEGRO_KEY_A && keyCode<=ALLEGRO_KEY_Z) 
					|| 	(keyCode>=ALLEGRO_KEY_0 && keyCode<=ALLEGRO_KEY_9))
						nameInput.insert(cursor++, al_keycode_to_name(keyCode));
					if(	keyCode==ALLEGRO_KEY_SPACE)
						nameInput.insert(cursor++, "_");
				}				
		}
		std::cerr<<"Key:"<<keyCode<<std::endl<<nameInput<<std::endl;
		PutName();
	}
	else{
		if(keyCode	==	ALLEGRO_KEY_ENTER){
			ChangeName();
		}
	}
}

void WinScene::PutName(){
	static const int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	static const int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	RemoveObject(nameLabel->GetObjectIterator());
	AddNewObject(nameLabel	= new Engine::Label(nameInput, "pirulen.ttf", 32, w/2, h/ 4 -30, 0, 0, 0, 255, 0.5, 0.5));
}
