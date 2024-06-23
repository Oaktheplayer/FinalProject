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
#include "Enemy/Enemy.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "UI/Component/Label.hpp"
#include "Turret/LaserTurret.hpp"
#include "Turret/Wall.hpp"
#include "Turret/Base.hpp"
#include "Turret/MachineGunTurret.hpp"
#include "Turret/MissileTurret.hpp"
#include "Turret/Flamethrower.hpp"
#include "UI/Animation/Plane.hpp"
#include "Enemy/PlaneEnemy.hpp"
#include "PlayScene.hpp"
#include "Engine/Resources.hpp"
#include "Enemy/SoldierEnemy.hpp"
#include "Enemy/TankEnemy.hpp"
#include "Enemy/ACV.hpp"
#include "Enemy/TruckEnemy.hpp"
#include "Enemy/SonicEnemy.h"
#include "Turret/TurretButton.hpp"
#include "Enemy/EnemyButton.h"
#include "Building/Building.hpp"


float scale;
const int opd[]={2,3,0,1,6,7,4,5};
bool PlayScene::DebugMode= false;
const std::vector<Engine::Point> PlayScene::directions = {
	Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1),
	Engine::Point(-1, -1), Engine::Point(1, -1), Engine::Point(1, 1), Engine::Point(-1, 1)};
const int PlayScene::BlockSize = 64;
const float PlayScene::DangerTime = 7.61;
const std::vector<int> PlayScene::code = { ALLEGRO_KEY_UP, ALLEGRO_KEY_UP, ALLEGRO_KEY_DOWN, ALLEGRO_KEY_DOWN,
									ALLEGRO_KEY_LEFT, ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT, ALLEGRO_KEY_RIGHT,
									ALLEGRO_KEY_B, ALLEGRO_KEY_A, ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_ENTER };
Engine::Point PlayScene::GetClientSize() {
	return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);
}
void PlayScene::Initialize() {
	scale=1;
	// DONE: [HACKATHON-3-BUG] (1/5): There's a bug in this file, which crashes the game when you lose. Try to find it.
	// DONE: [HACKATHON-3-BUG] (2/5): Find out the cheat code to test.
    // DONE: [HACKATHON-3-BUG] (2/5): It should generate a Plane, and add 10000 to the money, but it doesn't work now.
	keyStrokes.clear();
	ticks = 0;
	deathCountDown = -1;
	lives = 10;
	money = 1000;
	SpeedMult = 1;
	score = 0;
	scale 	= 	1;
	center	=	Point(Engine::GameEngine::GetInstance().GetScreenWidth()/2,Engine::GameEngine::GetInstance().GetScreenHeight()/2);
	sight	=	center;
	sight0	=	center;
	sight_dir	=	Point(0,0);
	sight_speed	=	16;
    gamemode = 0;
	// Add Map
	AddNewObject(MapComponent = new Group());
	// Add groups from bottom to top.
	MapComponent->AddNewObject(TileMapGroup = new Group());
	MapComponent->AddNewObject(GroundEffectGroup = new Group());
	MapComponent->AddNewObject(DebugIndicatorGroup = new Group());
	for(int i=0;i<TEAM_COUNT;i++){
		MapComponent->AddNewObject(UnitGroups[i] = new Group());
	}
	MapComponent->AddNewObject(BulletGroup = new Group());
	
	MapComponent->AddNewObject(EffectGroup = new Group());
	// Should support buttons.
	AddNewControlObject(UIGroup = new Group());
	ReadMap();
    //SpawnGridPoint = Engine::Point(-1, 0);
	SpawnGridPoint = Engine::Point(0, 0);
	if(gamemode==0){
        money=150;
        ReadEnemyWave();
    }
	mapDirection = std::vector<std::vector<char>>(MapHeight,	std::vector<char>	(MapWidth,-1));
	mapHValue = std::vector<std::vector<int>>(MapHeight, std::vector<int>	(MapWidth,-1));
	mapGCost = std::vector<std::vector<int>>(MapHeight, std::vector<int>	(MapWidth,-1));
	mapAStarVisited = std::vector<std::vector<bool>>(MapHeight, std::vector<bool>(MapWidth,false));
    ConstructUI();
	imgTarget = new Engine::Image("play/target.png", 0, 0);
	imgTarget->Visible = false;
	preview = nullptr;
	AddNewObject(imgTarget);
	// Preload Lose Scene
	deathBGMInstance = Engine::Resources::GetInstance().GetSampleInstance("astronomia.ogg");
	Engine::Resources::GetInstance().GetBitmap("lose/benjamin-happy.png");
	// Start BGM.
	bgmId = AudioHelper::PlayBGM("play.ogg");
    if(gamemode==1) {
        timer = al_create_timer(10);
        al_start_timer(timer);
    }
}
void PlayScene::Terminate() {
	AudioHelper::StopBGM(bgmId);
	AudioHelper::StopSample(deathBGMInstance);
	deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
	IScene::Terminate();
}
void PlayScene::Update(float deltaTime) {
	// If we use deltaTime directly, then we might have Bullet-through-paper problem.
	// Reference: Bullet-Through-Paper
    if(sight.x <=0)sight.x=0;
    else if(sight.x>=MapWidth*BlockSize)sight.x=MapWidth*BlockSize;
    if(sight.y <=0)sight.y=0;
    else if(sight.y>=MapHeight*BlockSize)sight.y=MapHeight*BlockSize;
	sight = sight + sight_dir*sight_speed;
    if(gamemode==0){
        if (SpeedMult == 0)
            deathCountDown = -1;
        std::vector<float> reachEndTimes;
        for (auto &it: UnitGroups[RED]->GetObjects()) {
            if (dynamic_cast<Enemy *>(it))
                reachEndTimes.push_back(dynamic_cast<Enemy *>(it)->reachEndTime);
        }
        // Can use Heap / Priority-Queue instead. But since we won't have too many enemies, sorting is fast enough.
        std::sort(reachEndTimes.begin(), reachEndTimes.end());
        float newDeathCountDown = -1;
        int danger = lives;
        for (auto &it: reachEndTimes) {
            if (it <= DangerTime) {
                danger--;
                if (danger <= 0) {
                    // Death Countdown
                    float pos = DangerTime - it;
                    if (it > deathCountDown) {
                        // Restart Death Count Down BGM.
                        AudioHelper::StopSample(deathBGMInstance);
                        if (SpeedMult != 0)
                            deathBGMInstance = AudioHelper::PlaySample("astronomia.ogg", false, AudioHelper::BGMVolume,
                                                                       pos);
                    }
                    float alpha = pos / DangerTime;
                    alpha = std::max(0, std::min(255, static_cast<int>(alpha * alpha * 255)));
                    dangerIndicator->Tint = al_map_rgba(255, 255, 255, alpha);
                    newDeathCountDown = it;
                    break;
                }
            }
        }
        deathCountDown = newDeathCountDown;
        if (SpeedMult == 0)
            AudioHelper::StopSample(deathBGMInstance);
        if (deathCountDown == -1 && lives > 0) {
            AudioHelper::StopSample(deathBGMInstance);
            dangerIndicator->Tint.a = 0;
        }
        if (SpeedMult == 0)
            deathCountDown = -1;
    }
	for (int i = 0; i < SpeedMult; i++) {
		IScene::Update(deltaTime);
		// Check if we should create new enemy.
		ticks += deltaTime;
        if(gamemode==0) {
            if (enemyWaveData.empty()) {
                if (UnitGroups[RED]->GetObjects().empty()) {
                    ScorePoint(lives ^ 2);
                    RecordScore();
                    Engine::GameEngine::GetInstance().ChangeScene("win");
                }
                continue;
            }
        }else if(gamemode==1){
            if(!BaseExsist){
                al_stop_timer(timer);
                score=(15-al_get_timer_count(timer))*100;
                if(score<=0)score=0;
                //al_destroy_timer(timer);
                RecordScore();
                Engine::GameEngine::GetInstance().ChangeScene("win");
            }
            continue;
        }

		auto current = enemyWaveData.front();
		if (ticks < current.second)
			continue;
		ticks -= current.second;
		enemyWaveData.pop_front();
		const Engine::Point SpawnCoordinate = Engine::Point(SpawnGridPoint.x * BlockSize + BlockSize / 2, SpawnGridPoint.y * BlockSize + BlockSize / 2);
		Enemy* enemy	=	SpawnEnemy(current.first,SpawnCoordinate.x,SpawnCoordinate.y, ticks);
		if( !enemy) continue;
		// Compensate the time lost.
		enemy->Update(ticks);
	}
	if (preview) {
		preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
		preview->Update(deltaTime);
	}
}
Enemy* PlayScene::SpawnEnemy(int type, float x, float y, float delta){
	Enemy* enemy;
	switch (type) {
	case 1:
		UnitGroups[RED]->AddNewObject(enemy = new SoldierEnemy(x, y,RED));
		break;
	case 2:
		UnitGroups[RED]->AddNewObject(enemy = new PlaneEnemy(x, y,RED));
		break;
	case 3:
		UnitGroups[RED]->AddNewObject(enemy = new TankEnemy(x, y,RED));
		break;
	case 4:
		UnitGroups[RED]->AddNewObject(enemy = new TruckEnemy(x, y,RED));
		break;
    case 5:
        UnitGroups[RED]->AddNewObject(enemy = new SonicEnemy(x, y,RED));
        break;
    case 6:
        UnitGroups[RED]->AddNewObject(enemy = new ACV(x, y,RED));
        break;
    // TODO: [CUSTOM-ENEMY]: You need to modify 'Resource/enemy1.txt', or 'Resource/enemy2.txt' to spawn the 4th enemy.
    //         The format is "[EnemyId] [TimeDelay] [Repeat]".
    // TODO: [CUSTOM-ENEMY]: Enable the creation of the enemy.
	default:
		return nullptr;
	}	
	enemy->UpdatePath(mapDistance);
	return enemy;
}
void PlayScene::Draw(float scale, float cx, float cy, float sx, float sy) const {
	//IScene::Draw();
    al_clear_to_color(al_map_rgb(0, 0, 0));
    MapComponent->Draw(this->scale, center.x, center.y, sight.x, sight.y);
//	if (DebugMode) {
//		// Draw reverse BFS distance on all reachable blocks.
//		for (int i = 0; i < MapHeight; i++) {
//			for (int j = 0; j < MapWidth; j++) {
//                float x = (j + 0.5) * BlockSize;
//                float y = (i + 0.5) * BlockSize;
//                Engine::Label label1(std::to_string(mapHValue[i][j]), "pirulen.ttf",16,x,y);
//                label1.Anchor = Engine::Point(0.5, 0.5);
//                label1.Draw(this->scale, center.x, center.y, sight.x, sight.y);
//				if (mapGCost[i][j] != -1) {
//					Engine::Label label2(std::to_string(mapGCost[i][j]), "pirulen.ttf",16,
//					x ,
//					y );
//					label2.Anchor = Engine::Point(0.5, 0.5);
//					label2.Draw(this->scale, center.x, center.y, sight.x, sight.y+10);
//				}
//			}
//		}
//	}
    UIGroup->Draw();
	if(imgTarget->Visible)	imgTarget->Draw(this->scale, center.x, center.y, sight.x, sight.y);
	if(preview)		{
        preview->Draw(this->scale,preview->Position.x,preview->Position.y,preview->Position.x,preview->Position.y);
    }
}
void PlayScene::OnMouseScroll(int mx, int my, int delta){
	float	pre_s	=	scale;
	scale+= (float)delta /4;
	if(scale>2)	scale=2;
	else if(scale<0.5)	scale=0.5;

	Point	mouse(mx,my);
	sight	=	(mouse-center)/pre_s + sight - (mouse-center)/scale;	
}
void PlayScene::OnMouseDown(int button, int mx, int my) {
	if ((button & 1) && !imgTarget->Visible && preview) {
		// Cancel turret construct.
		RemoveObject(preview->GetObjectIterator());
		preview = nullptr;
	}
	IScene::OnMouseDown(button, mx, my);
}
void PlayScene::OnMouseMove(int mx, int my) {
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
void PlayScene::OnMouseUp(int button, int mx, int my) {
	IScene::OnMouseUp(button, mx, my);
	if (!imgTarget->Visible)
		return;
	const int x = ((float)(mx-center.x)/scale + sight.x) 	/	BlockSize;
	const int y = ((float)(my-center.y)/scale + sight.y)	/	BlockSize;
	if (button & 1) {
		if (x>=0 && x<=MapWidth && y>=0 && y<=MapWidth && !mapBuildings[y][x]) {
			if (!preview || (UIGroup->Visible && mx>=Engine::GameEngine::GetInstance().GetScreenWidth()-320))
				return;
			// Check if valid.
			if (!CheckSpaceValid(x, y ,preview)) {

				Engine::Sprite* sprite;
				GroundEffectGroup->AddNewObject(sprite = new DirtyEffect("play/target-invalid.png", 1, x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2));
				sprite->Rotation = 0;
				return;
			}
			// Purchase.
			EarnMoney(-preview->GetPrice());
			// Remove Preview.
			preview->GetObjectIterator()->first = false;
			RemoveObject(preview->GetObjectIterator());
			// Construct real turret.
			preview->Position.x = x * BlockSize + BlockSize / 2;
			preview->Position.y = y * BlockSize + BlockSize / 2;
			preview->Enabled = true;
			preview->Preview = false;
			preview->Tint = al_map_rgba(255, 255, 255, 255);
            if(dynamic_cast<Building*>(preview)) {
                UnitGroups[BLUE]->AddNewObject(dynamic_cast<Building *>(preview));
                mapBuildings[y][x] = dynamic_cast<Building*>(preview);
            }else {
                UnitGroups[BLUE]->AddNewObject(dynamic_cast<Enemy *>(preview));
                dynamic_cast<Enemy *>(preview)->UpdatePath(mapDistance);
            }
			// To keep responding when paused.
			preview->Update(0);
			// Remove Preview.
			preview = nullptr;
			OnMouseMove(mx, my);
		}
	}
}
void PlayScene::OnKeyDown(int keyCode) {
	IScene::OnKeyDown(keyCode);
	if (keyCode == ALLEGRO_KEY_TAB) {
		DebugMode = !DebugMode;
	}
	else {
		keyStrokes.push_back(keyCode);
		if (keyStrokes.size() > code.size())
			keyStrokes.pop_front();
		if (keyCode == ALLEGRO_KEY_ENTER && keyStrokes.size() == code.size()) {
			auto it = keyStrokes.begin();
			for (int c : code) {
				if (!((*it == c) ||
					(c == ALLEGRO_KEYMOD_SHIFT &&
					(*it == ALLEGRO_KEY_LSHIFT || *it == ALLEGRO_KEY_RSHIFT))))
					return;
				++it;
			}
			EffectGroup->AddNewObject(new Plane());
			money+=10000;
		}
	}
	// TODO: [CUSTOM-TURRET]: Make specific key to create the turret.
	if (keyCode == ALLEGRO_KEY_1) {
		if(gamemode==0){
			// Hotkey for MachineGunTurret.
			DMUIBtnClicked(0);
		}else
		if(gamemode==1){AMUIBtnClicked(0);}
	}
	else if (keyCode == ALLEGRO_KEY_2) {
		if(gamemode==0){
			// Hotkey for LaserTurret.
			DMUIBtnClicked(1);
		}else
		if(gamemode==1){AMUIBtnClicked(1);}
	}
	else if (keyCode == ALLEGRO_KEY_3) {
		if(gamemode==0){
			// Hotkey for MissileTurret.
			DMUIBtnClicked(2);
		}else
		if(gamemode==1){AMUIBtnClicked(2);}
	}
	else if (keyCode == ALLEGRO_KEY_4) {
		if(gamemode==0){
			// Hotkey for Flamethrower.
			DMUIBtnClicked(3);
		}else
		if(gamemode==1){AMUIBtnClicked(3);}
	}
	else if (keyCode == ALLEGRO_KEY_5) {
		if(gamemode==0){
			// Hotkey for Wall.
			DMUIBtnClicked(4);
		}else
		if(gamemode==1){AMUIBtnClicked(4);}
	}else if (keyCode == ALLEGRO_KEY_6) {
		if(gamemode==0){
			// Hotkey for Wall.
			DMUIBtnClicked(5);
		}else
		if(gamemode==1){AMUIBtnClicked(5);}
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
	else if(keyCode	==	ALLEGRO_KEY_UP){
		SpeedMult++;
		if(SpeedMult>20)SpeedMult=20;
	}
	else if(keyCode	==	ALLEGRO_KEY_DOWN){
		SpeedMult--;
		if(SpeedMult<0)SpeedMult=0;
	}
	else if(keyCode == ALLEGRO_KEY_X){
		scale=1;
		sight	=	sight0;
	}
}
void PlayScene::OnKeyUp(int keyCode){
	if(keyCode	==	ALLEGRO_KEY_W || keyCode	==	ALLEGRO_KEY_S){
		sight_dir.y=0;
	}
	if(keyCode	==	ALLEGRO_KEY_A || keyCode	==	ALLEGRO_KEY_D){
		sight_dir.x=0;
	}
}
void PlayScene::Hit() {
    if(gamemode==0) {
        if (lives > 0)lives--;
        UILives->Text = std::string("Life ") + std::to_string(lives);
        if (lives <= 0 ) {
            Engine::GameEngine::GetInstance().ChangeScene("lose");
        }
    }
}
int PlayScene::GetMoney() const {
	return money;
}
void PlayScene::EarnMoney(int money) {
	this->money += money;
	UIMoney->Text = std::string("$") + std::to_string(this->money);
}
void PlayScene::ReadMap() {
	
	std::string filename = std::string("../Resource/map") + std::to_string(StageId*10+MapId) + ".txt";
	// Read map file.
	char c;
	std::vector<int> mapData;
	std::ifstream fin(filename);
	fin>>MapWidth;
	fin>>MapHeight;
    fin>>gamemode;
    EndGridPoint = Engine::Point(MapWidth-1, MapHeight - 1);
	while (fin >> c) {
		switch (c) {
		case '\n':
		case '\r':
			if (static_cast<int>(mapData.size()) / MapWidth != 0)
				throw std::ios_base::failure("Map data is corrupted1.");
			break;
		default:
            if(c>='0'&&c<='9')mapData.push_back(c-'0');
            else throw std::ios_base::failure("Map data is corrupted2.");
		}
	}
	std::cerr<<MapWidth<<','<<MapHeight<<'\n';
	fin.close();
	// Validate map data.
    if(gamemode==0){
        if (static_cast<int>(mapData.size()) != MapWidth * MapHeight)throw std::ios_base::failure("Map data is corrupted3.");
    }else{
        if (static_cast<int>(mapData.size()) != MapWidth * MapHeight*2)throw std::ios_base::failure("Map data is corrupted4.");
    }
    // Store map in 2d array.
	mapTerrain = std::vector<std::vector<TileType>>(MapHeight, std::vector<TileType>(MapWidth));
	for (int i = 0; i < MapHeight; i++) {
		for (int j = 0; j < MapWidth; j++) {
			const int num = mapData[i * MapWidth + j];
            switch(num){
                case 0:
                    mapTerrain[i][j]=TILE_DIRT;
                    TileMapGroup->AddNewObject(new Engine::Image("play/dirt.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                    break;
                case 1:
                    mapTerrain[i][j]=TILE_FLOOR;
                    TileMapGroup->AddNewObject(new Engine::Image("play/grass.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                    break;
                case 2:
                    mapTerrain[i][j]=TILE_WATER;
                    TileMapGroup->AddNewObject(new Engine::Image("play/water.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                    break;
                default:
                    std::cerr<<"unknown map terrain\n";
                    break;            }
		}
	}
	//TEST
	//mapBuildings	=	std::vector<std::vector<Turret*>>(MapHeight, std::vector<Turret*>(MapWidth,nullptr));
    mapBuildings	=	std::vector<std::vector<Building*>>(MapHeight, std::vector<Building*>(MapWidth,nullptr));
    if(gamemode==1){
        int BaseCnt=0;
        for (int i = MapHeight; i < 2*MapHeight; i++) {
            for (int j = 0; j < MapWidth; j++) {
                const int num = mapData[i * MapWidth + j];
                switch(num){
                    case 0:
                        break;
                    case 1:
                        UnitGroups[RED]->AddNewObject(mapBuildings[i-MapHeight][j]=new Wall((j+0.5)*BlockSize, (i-MapHeight+0.5)*BlockSize,RED));
                        break;
                    case 2:
                        UnitGroups[RED]->AddNewObject(mapBuildings[i-MapHeight][j]=new LaserTurret((j+0.5)*BlockSize, (i-MapHeight+0.5)*BlockSize,RED));
                        break;
                    case 3:
                        UnitGroups[RED]->AddNewObject(mapBuildings[i-MapHeight][j]=new MissileTurret((j+0.5)*BlockSize, (i-MapHeight+0.5)*BlockSize,RED));
                        break;
                    case 4:
                        UnitGroups[RED]->AddNewObject(mapBuildings[i-MapHeight][j]=new Flamethrower((j+0.5)*BlockSize, (i-MapHeight+0.5)*BlockSize,RED));
                        break;
                    case 5:
                        UnitGroups[RED]->AddNewObject(mapBuildings[i-MapHeight][j]=new MachineGunTurret((j+0.5)*BlockSize, (i-MapHeight+0.5)*BlockSize,RED));
                        break;
                    case 6:
                        UnitGroups[RED]->AddNewObject(mapBuildings[i-MapHeight][j]=new Base((j+0.5)*BlockSize, (i-MapHeight+0.5)*BlockSize,RED));
                        EndGridPoint = Engine::Point(j, i-MapHeight);
                        BaseExsist=true;
                        BaseCnt++;
                        break;
                    default:
                        std::cerr<<"unknown build type\n";
                        break;
                }
            }
        }
        if(BaseCnt==0)std::cerr<<"missing Base\n";
        else if(BaseCnt>1)std::cerr<<"Too many Base\n";
    }
    std::cerr<<"map read succeed\n";
}
void PlayScene::ReadEnemyWave() {
    // DONE: [HACKATHON-3-BUG] (3/5): Trace the code to know how the enemies are created.
    // DONE: [HACKATHON-3-BUG] (3/5): There is a bug in these files, which let the game only spawn the first enemy, try to fix it.
    std::string filename = std::string("Resource/enemy") + std::to_string(StageId*10+ MapId) + ".txt";
	// Read enemy file.
	float type, wait, repeat;
	enemyWaveData.clear();
	std::ifstream fin(filename);
	while (fin >> type && fin >> wait && fin >> repeat) {
		for (int i = 0; i < repeat; i++)
			enemyWaveData.emplace_back(type, wait);
	}
	fin.close();
}
void PlayScene::ConstructUI() {
	// Background
	UIGroup->AddNewObject(new Engine::Image("play/sand.png", 1280, 0, 320, 832));
	// Text
	UIGroup->AddNewObject(new Engine::Label(std::string("Stage ") + std::to_string(MapId), "pirulen.ttf", 32, 1294, 0));
	UIGroup->AddNewObject(UIMoney = new Engine::Label(std::string("$") + std::to_string(money), "pirulen.ttf", 24, 1294, 48));
	UIGroup->AddNewObject(UIScore = new Engine::Label(std::string("Score ") + std::to_string(score), "pirulen.ttf", 24, 1294,	88));

    if(gamemode==0){
        UIGroup->AddNewObject(UILives = new Engine::Label(std::string("Life ") + std::to_string(lives), "pirulen.ttf", 24, 1294, 128));
        DefenceModeUI();
    }
    else AttackModeUI();

	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int shift = 135 + 25;
	dangerIndicator = new Engine::Sprite("play/benjamin.png", w - shift, h - shift);
	dangerIndicator->Tint.a = 0;
	UIGroup->AddNewObject(dangerIndicator);
}
void PlayScene::AttackModeUI(){
    EnemyButton* btn;
    int y = 176;
    int x = 1294;
    int dx = 1370-1294;
    int i = 0;
    // Button 0
    btn = new EnemyButton("play/button1.png", "play/button2.png",
                          Engine::Sprite("play/enemy-1.png", x+i%4*dx+24, y+(i/4)*dx+24, 0, 0, 0, 0)
            , x+i%4*dx, y+(i/4)*dx, 10);
    btn->SetOnClickCallback(std::bind(&PlayScene::AMUIBtnClicked, this, 0));
    UIGroup->AddNewControlObject(btn);
    i++;
    // Button 1
    btn = new EnemyButton("play/button1.png", "play/button2.png",
                          Engine::Sprite("play/enemy-3.png", x+i%4*dx, y+(i/4)*dx, 0, 0, 0, 0)
            , x+i%4*dx, y+(i/4)*dx, 10);
    btn->SetOnClickCallback(std::bind(&PlayScene::AMUIBtnClicked, this, 1));
    UIGroup->AddNewControlObject(btn);
    i++;
    // Button 2
    btn = new EnemyButton("play/button1.png", "play/button2.png",
                          Engine::Sprite("play/truck_troop.png", x+i%4*dx-8, y+(i/4)*dx+12, 0, 0, 0, 0)
            , x+i%4*dx, y+(i/4)*dx, 10);
    btn->SetOnClickCallback(std::bind(&PlayScene::AMUIBtnClicked, this, 2));
    UIGroup->AddNewControlObject(btn);
    i++;
    // Button 3
    btn = new EnemyButton("play/button1.png", "play/button2.png",
                          Engine::Sprite("play/enemy-2.png", x+i%4*dx, y+(i/4)*dx, 0, 0, 0, 0)
            , x+i%4*dx, y+(i/4)*dx, 10);
    btn->SetOnClickCallback(std::bind(&PlayScene::AMUIBtnClicked, this, 3));
    UIGroup->AddNewControlObject(btn);
    i++;
    // Button 4
    btn = new EnemyButton("play/button1.png", "play/button2.png",
                           Engine::Sprite("play/jeep_troop.png", x+i%4*dx, y+(i/4)*dx, 0, 0, 0, 0)
            , x+i%4*dx, y+(i/4)*dx, 10);
    btn->SetOnClickCallback(std::bind(&PlayScene::AMUIBtnClicked, this, 4));
    UIGroup->AddNewControlObject(btn);
    i++;
	// Button 5
    btn = new EnemyButton("play/button1.png", "play/button2.png",
                           Engine::Sprite("play/acv_troop.png", x+i%4*dx-25, y+(i/4)*dx+8, 0, 0, 0, 0)
            , x+i%4*dx, y+(i/4)*dx, 10);
    btn->SetOnClickCallback(std::bind(&PlayScene::AMUIBtnClicked, this, 5));
    UIGroup->AddNewControlObject(btn);
}
void PlayScene::DefenceModeUI(){
    TurretButton* btn;
    int y = 176;
    int x = 1294;
    int dx = 1370-1294;
    int i = 0;
    // Button 0
    btn = new TurretButton("play/button1.png", "play/button2.png",
                           Engine::Sprite("play/tower-base.png", x+i%4*dx, y, 0, 0, 0, 0),
                           Engine::Sprite("play/turret-1.png", x+i%4*dx, y - 8, 0, 0, 0, 0)
            , x+i%4*dx, y, MachineGunTurret::Price);
    btn->SetOnClickCallback(std::bind(&PlayScene::DMUIBtnClicked, this, 0));
    UIGroup->AddNewControlObject(btn);
    i++;
    // Button 1
    btn = new TurretButton("play/button1.png", "play/button2.png",
                           Engine::Sprite("play/tower-base.png", x+i%4*dx, y, 0, 0, 0, 0),
                           Engine::Sprite("play/turret-2.png", x+i%4*dx, y - 8, 0, 0, 0, 0)
            , x+i%4*dx, y, LaserTurret::Price);
    btn->SetOnClickCallback(std::bind(&PlayScene::DMUIBtnClicked, this, 1));
    UIGroup->AddNewControlObject(btn);
    i++;
    // Button 2
    btn = new TurretButton("play/button1.png", "play/button2.png",
                           Engine::Sprite("play/tower-base.png", 1446, y, 0, 0, 0, 0),
                           Engine::Sprite("play/turret-3.png", 1446, y - 8, 0, 0, 0, 0)
            , x+i%4*dx, y, MissileTurret::Price);
    btn->SetOnClickCallback(std::bind(&PlayScene::DMUIBtnClicked, this, 2));
    UIGroup->AddNewControlObject(btn);
    i++;
    // Button 3
    btn = new TurretButton("play/button1.png", "play/button2.png",
                           Engine::Sprite("play/tower-base.png", x+i%4*dx, y, 0, 0, 0, 0),
                           Engine::Sprite("play/turret-6.png", x+i%4*dx, y - 8, 0, 0, 0, 0)
            , x+i%4*dx, y, Flamethrower::Price);
    btn->SetOnClickCallback(std::bind(&PlayScene::DMUIBtnClicked, this, 3));
    UIGroup->AddNewControlObject(btn);
    i++;
    // Button 4
    btn = new TurretButton("play/button1.png", "play/button2.png",
                           Engine::Sprite("play/tower-base.png", x+i%4*dx, y+(i/4)*dx, 0, 0, 0, 0),
                           Engine::Sprite("play/tower-base.png", x+i%4*dx, y+(i/4)*dx, 0, 0, 0, 0)
            , x+i%4*dx, y+(i/4)*dx, Wall::Price);
    btn->SetOnClickCallback(std::bind(&PlayScene::DMUIBtnClicked, this, 4));
    UIGroup->AddNewControlObject(btn);
    i++;
}
void PlayScene::AMUIBtnClicked(int id) {
    if (preview){
        RemoveObject(preview->GetObjectIterator());
    }
    if (id == 0 && money >= SoldierEnemy::Price) {
        preview = new SoldierEnemy(0, 0,BLUE);
    }else if (id == 1 && money >= TankEnemy::Price) {
        preview = new TankEnemy(0, 0,BLUE);
    }else if (id == 2 && money >= TruckEnemy::Price) {
        preview = new TruckEnemy(0, 0,BLUE);
    }else if (id == 3 && money >= PlaneEnemy::Price) {
        preview = new PlaneEnemy(0, 0,BLUE);
    }else if (id == 4 && money >= SonicEnemy::Price) {
        preview = new SonicEnemy(0, 0,BLUE);
    }else if (id == 5 && money >= SonicEnemy::Price) {
        preview = new ACV(0, 0,BLUE);
    }
    else preview=nullptr;
    if (!preview){
        imgTarget->Visible=false;
        return;
    }
    preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
    preview->Tint = al_map_rgba(255, 255, 255, 200);
    preview->Enabled = false;
    preview->Preview = true;
    AddNewObject(preview);
    OnMouseMove(Engine::GameEngine::GetInstance().GetMousePosition().x, Engine::GameEngine::GetInstance().GetMousePosition().y);
}
void PlayScene::DMUIBtnClicked(int id) {
	if (preview){
		RemoveObject(preview->GetObjectIterator());
	}
    // TODO: [CUSTOM-TURRET]: On callback, create the turret.
	if (id == 0 && money >= MachineGunTurret::Price)
		preview = new MachineGunTurret(0, 0,BLUE);
	else if (id == 1 && money >= LaserTurret::Price)
		preview = new LaserTurret(0, 0,BLUE);
	else if (id == 2 && money >= MissileTurret::Price)
		preview = new MissileTurret(0, 0,BLUE);
	else if (id == 3 && money >= Flamethrower::Price)
		preview = new Flamethrower(0, 0,BLUE);
    else if (id == 4 && money >= Wall::Price)
        preview = new Wall(0, 0,BLUE);
	else preview=nullptr;
	if (!preview){
		imgTarget->Visible=false;
		return;
	}
	preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
	preview->Tint = al_map_rgba(255, 255, 255, 200);
	preview->Enabled = false;
	preview->Preview = true;
	AddNewObject(preview);
	OnMouseMove(Engine::GameEngine::GetInstance().GetMousePosition().x, Engine::GameEngine::GetInstance().GetMousePosition().y);
}
bool PlayScene::CheckSpaceValid(int x, int y,Unit* unit) {
	if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)return false;
    if(dynamic_cast<Enemy*>(unit)) {
        if(mapTerrain[y][x]!=TILE_DIRT)return false;
        return true;
    }else{
        mapBuildings[y][x] = dynamic_cast<Building*>(unit);
		//TEST
		bool hasPath = false;
        if(mapDirection[y][x]!=-1) hasPath	=	true;
		for(int i=0;i<4 && !hasPath;i++){
			Point dir = directions[i];
			if(x+dir.x<0||x+dir.x>=MapWidth|| y+dir.y<0||y+dir.y>=MapHeight)continue;
			if(mapDirection[y+dir.y][x+dir.x]!=-1){
				hasPath = true;
				break;
			}
		}
		if(!hasPath)	return true;
        mapAStarVisited= std::vector<std::vector<bool>>(MapHeight, std::vector<bool>(MapWidth,false));
        //mapDirection= std::vector<std::vector<char>>(MapHeight, std::vector<char>(MapWidth,-1));
        mapDirection[y][x]=-1;
		for(auto& g: UnitGroups)
        for (auto& it : g->GetObjects())if(dynamic_cast<Enemy*>(it))
                dynamic_cast<Enemy*>(it)->UpdatePath(mapDistance);
        return true;
    }
}

std::string PlayScene::AStarPathFinding(Point start, int flag)
{
	Point end = EndGridPoint;
    const float G=15.0f/(float)BlockSize;
	std::priority_queue<PathData> Q;
	std::vector<std::vector<bool>>mapAStarVisited= std::vector<std::vector<bool>>(MapHeight, std::vector<bool>(MapWidth,false));
	Q.push(PathData(start,0,HVal(start,end),"",-1));
	//std::cerr<<"pathfinding started\n";
	std::string path_str="";
	while(!Q.empty()){
		PathData curstate = Q.top();
		if(curstate	==	end){
			path_str=curstate.path;
			break;
		}
		Q.pop();
		int dirToken = mapDirection[curstate.y][curstate.x];
		if((!(curstate.repathing)) && dirToken != -1){
				//TEST
				PathData S(curstate);
				S.repathing	=	true;
				Q.push(S);
			Point next(curstate+directions[dirToken]);
			if(!CanPassGridPoint(next.x,next.y,flag,true)) continue;
			int g = curstate.g_cost;
			if(dirToken>=4){
				if(HasBuildingAt(next.x,curstate.y) || HasBuildingAt(curstate.x,next.y)) continue;
				g+=14;
			}
			else g += 10;
			if(HasBuildingAt(next.x,next.y)){
				g += mapBuildings[next.y][next.x]->GetHp()*G;
			}
			//std::cerr<<next.x<<','<<next.y<<'\n';
			PathData N = PathData(next,g,HVal(next,end),curstate.path,mapDirection[curstate.y][curstate.x]);
			N.path.push_back('0'+mapDirection[curstate.y][curstate.x]);
			Q.push(N);
            //std::cerr<<"2bruh: "<<N.path<<'\n';
			continue;
		}

		if( mapAStarVisited[curstate.y][curstate.x]) continue;
		else mapAStarVisited[curstate.y][curstate.x]=true;

		//std::cerr<<"bruh: "<<curstate.path<<'\n';
		for(int i=0;i<8;i++){
			if(opd[i]==curstate.premove)	continue;
			if(curstate.repathing && i == mapDirection[curstate.y][curstate.x]) continue;
			Point next(curstate+directions[i]);
			if(next.x<0 || next.x>=MapWidth || next.y<0	|| next.y>=MapHeight
			|| !CanPassGridPoint(next.x,next.y,flag,true))
				continue;
            int g	=	curstate.g_cost;
            if(i>=4){
                // if(	mapTerrain[next.y][curstate.x]!=TILE_DIRT||	mapTerrain[curstate.y][next.x]!=TILE_DIRT
				// ||  HasBuildingAt(next.x,curstate.y) || HasBuildingAt(curstate.x,next.y))
				if(!CanPassGridPoint(next.x,curstate.y,flag)	||	!CanPassGridPoint(curstate.x,next.y,flag))
					continue;
                g+=14;
            }else{
                g+=10;
            }
            if(HasBuildingAt(next.x,next.y)){
                g	+=	mapBuildings[next.y][next.x]->GetHp()*G;
            }
            //std::cerr<<next.x<<','<<next.y<<'\n';
            PathData N = PathData(next,g,HVal(next,end),curstate.path,i);
            mapGCost[next.y][next.x] = N.g_cost;
            N.path.push_back('0'+i);
            Q.push(N);
            // Q.push(PathData(next,g,HVal(next,end),state.path,i));
            //std::cerr<<PathData(next,g,HVal(next,end),state.path,i).x<<','<<PathData(next,g,HVal(next,end),state.path,i).y<<'\n';
		}
	}
	if(Q.empty()){
		std::cerr<<"pathfinding error\n";
    	return std::string();
	}
	return	path_str;
}

bool PlayScene::CanPassGridPoint(int x, int y, int flag, bool ignoreBuilding)
{
	if(!ignoreBuilding && HasBuildingAt(x,y))	return	false;
	if(mapTerrain[y][x]==TILE_FLOOR)			return	false;
	
	if((flag==Ground || flag==Amph)	&&	mapTerrain[y][x]==TILE_DIRT)
		return	true;
	if((flag==Water	 || flag==Amph)	&&	mapTerrain[y][x]==TILE_WATER)
		return	true;

    return false;
}

int PlayScene::HVal(Point A, Point B)
{
	if(A==B)return 0;
    if(mapHValue[A.y][A.x]!=-1)return mapHValue[A.y][A.x];
    else{
        Point D(abs((int)A.x-(int)B.x),abs((int)A.y-(int)B.y));
        return mapHValue[A.y][A.x]=std::min((int)D.x,(int)D.y)*14 + abs((int)D.x- (int)D.y)*10;
    }
}

void PlayScene::RemoveBuilding(int x, int y){
    std::cerr<<"start removing building\n";
	mapBuildings[y][x]=nullptr;
	if(mapTerrain[y][x]!=TILE_FLOOR){
		mapAStarVisited= std::vector<std::vector<bool>>(MapHeight, std::vector<bool>(MapWidth,false));
        mapDirection[y][x]=-1;
		for(auto& g: UnitGroups)
			for (auto& it : g->GetObjects())if(dynamic_cast<Enemy*>(it))dynamic_cast<Enemy*>(it)->UpdatePath(mapDistance);
	}
	std::cerr<<"successfully removed building\n";
}
Building *PlayScene::HasBuildingAt(int x, int y)
{
//	if(x == EndGridPoint.x && y == EndGridPoint.y)	return	nullptr;
	if(	x<0 || 	x>=MapWidth||	y<0	||	y>=MapHeight)return nullptr;
	return mapBuildings[y][x];
}
Building *PlayScene::HasBuildingAt(Point p){
	return HasBuildingAt(p.x,p.y);
}

void PlayScene::ScorePoint(int point){
	score+=point;
	UIScore->Text = "Score "+std::to_string(score);
}
void PlayScene::RecordScore()
{
    std::ofstream   fout("Resource/currentdata.txt");
    fout << score;
}

