#include <allegro5/allegro.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <vector>
#include <queue>
#include <string>
#include <memory>

#include "Engine/AudioHelper.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "Enemy/Enemy.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "UI/Component/Label.hpp"
#include "Turret/LaserTurret.hpp"
#include "Turret/Wall.hpp"
#include "Turret/MachineGunTurret.hpp"
#include "Turret/MissileTurret.hpp"
#include "Turret/Flamethrower.hpp"
#include "UI/Animation/Plane.hpp"
#include "Enemy/PlaneEnemy.hpp"
#include "PlayScene.hpp"
#include "Engine/Resources.hpp"
#include "Enemy/SoldierEnemy.hpp"
#include "Enemy/TankEnemy.hpp"
#include "Enemy/TruckEnemy.hpp"
#include "Enemy/SonicEnemy.h"
#include "Turret/TurretButton.hpp"
//#include "Engine/Unit.hpp"

#include	<iostream>

float scale;
const int opd[]={2,3,0,1,6,7,4,5};

bool PlayScene::DebugMode = false;
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
	money = 150;
	SpeedMult = 1;
	score = 0;
	scale 	= 	1;
	center	=	Point(Engine::GameEngine::GetInstance().GetScreenWidth()/2,Engine::GameEngine::GetInstance().GetScreenHeight()/2);
	sight	=	center;
	sight0	=	center;
	sight_dir	=	Point(0,0);
	sight_speed	=	16;
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
	ReadEnemyWave();
	mapDirection= std::vector<std::vector<char>>(MapHeight,	std::vector<char>	(MapWidth,-1));
	mapHValue	= std::vector<std::vector<int>>	(MapHeight, std::vector<int>	(MapWidth,-1));
	// mapDistance = CalculateBFSDistance(0);
    // if(mapDistance[0][0]  ==-1)mapDistance = CalculateBFSDistance(1);
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
	if (SpeedMult == 0)
		deathCountDown = -1;
//	else if (deathCountDown != -1)
//		SpeedMult = 1;
	// Calculate danger zone.
	std::vector<float> reachEndTimes;
	for (auto& it : UnitGroups[RED]->GetObjects()) {
        if(dynamic_cast<Enemy*>(it))
		reachEndTimes.push_back(dynamic_cast<Enemy*>(it)->reachEndTime);
	}
	// Can use Heap / Priority-Queue instead. But since we won't have too many enemies, sorting is fast enough.
	std::sort(reachEndTimes.begin(), reachEndTimes.end());
	float newDeathCountDown = -1;
	int danger = lives;
	for (auto& it : reachEndTimes) {
		if (it <= DangerTime) {
			danger--;
			if (danger <= 0) {
				// Death Countdown
				float pos = DangerTime - it;
				if (it > deathCountDown) {
					// Restart Death Count Down BGM.
					AudioHelper::StopSample(deathBGMInstance);
					if (SpeedMult != 0)
						deathBGMInstance = AudioHelper::PlaySample("astronomia.ogg", false, AudioHelper::BGMVolume, pos);
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
	for (int i = 0; i < SpeedMult; i++) {
		IScene::Update(deltaTime);
		// Check if we should create new enemy.
		ticks += deltaTime;
		if (enemyWaveData.empty()) {
			if (UnitGroups[RED]->GetObjects().empty()) {
				ScorePoint(lives^2);
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
		// To keep responding when paused.
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
	if (DebugMode) {
		// Draw reverse BFS distance on all reachable blocks.
		for (int i = 0; i < MapHeight; i++) {
			for (int j = 0; j < MapWidth; j++) {
				if (mapHValue[i][j] != -1) {
					// Not elegant nor efficient, but it's quite enough for debugging.
					int x = (j + 0.5) * BlockSize;
					int y = (i + 0.5) * BlockSize;
					Engine::Label label(std::to_string(mapHValue[i][j]), "pirulen.ttf", 32,
					(j + 0.5) * BlockSize,
					(i + 0.5) * BlockSize);
					label.Anchor = Engine::Point(0.5, 0.5);
					label.Draw(this->scale, center.x, center.y, sight.x, sight.y);
				}
			}
		}
	}
    UIGroup->Draw();
	if(imgTarget->Visible)	imgTarget->Draw(this->scale, center.x, center.y, sight.x, sight.y);
	if(preview)				preview->Draw(this->scale,preview->Position.x,preview->Position.y,preview->Position.x,preview->Position.y);
	

}
void PlayScene::OnMouseScroll(int mx, int my, int delta){
	float	pre_s	=	scale;
	scale+= (float)delta /4;
	if(scale>2)		scale=2;
	else
	if(scale<0.75)	scale=0.75;

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
			UnitGroups[BLUE]->AddNewObject(preview);
			mapBuildings[y][x]=preview;
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
	if (keyCode == ALLEGRO_KEY_1) {
		// Hotkey for MachineGunTurret.
		UIBtnClicked(0);
	}
	else if (keyCode == ALLEGRO_KEY_2) {
		// Hotkey for LaserTurret.
		UIBtnClicked(1);
	}
	else if (keyCode == ALLEGRO_KEY_3) {
		// Hotkey for MissileTurret.
		UIBtnClicked(2);
	}
	else if (keyCode == ALLEGRO_KEY_4) {
		// Hotkey for Flamethrower.
		UIBtnClicked(3);
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
	
	// // TODO: [CUSTOM-TURRET]: Make specific key to create the turret.
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
	lives--;
	UILives->Text = std::string("Life ") + std::to_string(lives);
	if (lives <= 0) {
		Engine::GameEngine::GetInstance().ChangeScene("lose");
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
	std::string filename = std::string("Resource/map") + std::to_string(MapId) + ".txt";
	// Read map file.
	char c;
    int gamemode;
	std::vector<int> mapData;
	std::ifstream fin(filename);
	fin>>MapWidth;
	fin>>MapHeight;
    fin>>gamemode;
    EndGridPoint = Engine::Point(MapWidth-1, MapHeight - 1);
	while (fin >> c) {
		switch (c) {
		case '0': mapData.push_back(0);	break;
		case '1': mapData.push_back(1); break;
        case '6': mapData.push_back(6); break;
		case '\n':
		case '\r':
			if (static_cast<int>(mapData.size()) / MapWidth != 0)
				throw std::ios_base::failure("Map data is corrupted1.");
			break;
		default: throw std::ios_base::failure("Map data is corrupted2.");
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
    mapBuildings	=	std::vector<std::vector<Turret*>>(MapHeight, std::vector<Turret*>(MapWidth,nullptr));
    if(gamemode==1){
        for (int i = MapHeight; i < 2*MapHeight; i++) {
            for (int j = 0; j < MapWidth; j++) {
                const int num = mapData[i * MapWidth + j];
                switch(num){
                    case 0:
                        break;
                    case 1:
                        UnitGroups[RED]->AddNewObject(mapBuildings[i-MapHeight][j]=new wall((j+0.5)*BlockSize, (i-MapHeight+0.5)*BlockSize,RED));
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
                        EndGridPoint = Engine::Point(j, i-MapHeight);
                        break;
                    default:
                        preview=nullptr;
                        std::cerr<<"unknown build type\n";
                        break;
                }
            }
        }
    }
    std::cerr<<"map read succeed\n";
}
void PlayScene::ReadEnemyWave() {
    // DONE: [HACKATHON-3-BUG] (3/5): Trace the code to know how the enemies are created.
    // DONE: [HACKATHON-3-BUG] (3/5): There is a bug in these files, which let the game only spawn the first enemy, try to fix it.
    std::string filename = std::string("Resource/enemy") + std::to_string(MapId) + ".txt";
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
	UIGroup->AddNewObject(UILives = new Engine::Label(std::string("Life ") + std::to_string(lives), "pirulen.ttf", 24, 1294, 88));
	UIGroup->AddNewObject(UIScore = new Engine::Label(std::string("Score ") + std::to_string(score), "pirulen.ttf", 24, 1294,	128));

	TurretButton* btn;
	int y = 176;
	int x = 1294;
	int dx = 1370-1294;
	int i = 0;
	// Button 1
	btn = new TurretButton("play/button1.png", "play/button2.png",
		Engine::Sprite("play/tower-base.png", x+i%4*dx, y, 0, 0, 0, 0),
		Engine::Sprite("play/turret-1.png", x+i%4*dx, y - 8, 0, 0, 0, 0)
		, x+i%4*dx, y, MachineGunTurret::Price);
	// Reference: Class Member Function Pointer and std::bind.
	btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 0));
	UIGroup->AddNewControlObject(btn);
	i++;
	// Button 2
	btn = new TurretButton("play/button1.png", "play/button2.png",
		Engine::Sprite("play/tower-base.png", x+i%4*dx, y, 0, 0, 0, 0),
		Engine::Sprite("play/turret-2.png", x+i%4*dx, y - 8, 0, 0, 0, 0)
		, x+i%4*dx, y, LaserTurret::Price);
	btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 1));
	UIGroup->AddNewControlObject(btn);
	i++;
	// Button 3
	btn = new TurretButton("play/button1.png", "play/button2.png",
		Engine::Sprite("play/tower-base.png", 1446, y, 0, 0, 0, 0),
		Engine::Sprite("play/turret-3.png", 1446, y - 8, 0, 0, 0, 0)
		, x+i%4*dx, y, MissileTurret::Price);
	btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 2));
	UIGroup->AddNewControlObject(btn);
	i++;
    // Button 4
	btn = new TurretButton("play/button1.png", "play/button2.png",
		Engine::Sprite("play/tower-base.png", x+i%4*dx, y, 0, 0, 0, 0),
		Engine::Sprite("play/turret-6.png", x+i%4*dx, y - 8, 0, 0, 0, 0)
		, x+i%4*dx, y, Flamethrower::Price);
	btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 3));
	UIGroup->AddNewControlObject(btn);
	i++;
    // Button 5
    btn = new TurretButton("play/button1.png", "play/button2.png",
                           Engine::Sprite("play/tower-base.png", x+i%4*dx, y+(i/4)*dx, 0, 0, 0, 0),
                           Engine::Sprite("play/tower-base.png", x+i%4*dx, y+(i/4)*dx, 0, 0, 0, 0)
            , x+i%4*dx, y+(i/4)*dx, wall::Price);
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 4));
    UIGroup->AddNewControlObject(btn);
    i++;

    btn = new TurretButton("play/button1.png", "play/button2.png",
                           Engine::Sprite("play/jeep_troop.png", x+i%4*dx, y+(i/4)*dx, 0, 0, 0, 0),
                           Engine::Sprite("play/jeep_troop.png", x+i%4*dx, y+(i/4)*dx, 0, 0, 0, 0)
            , x+i%4*dx, y+(i/4)*dx, 10);
    // Reference: Class Member Function Pointer and std::bind.
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 5));
    UIGroup->AddNewControlObject(btn);
    i++;
	// TODO: [CUSTOM-TURRET]: Create a button to support constructing the turret.

	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int shift = 135 + 25;
	dangerIndicator = new Engine::Sprite("play/benjamin.png", w - shift, h - shift);
	dangerIndicator->Tint.a = 0;
	UIGroup->AddNewObject(dangerIndicator);
}
void PlayScene::UIBtnClicked(int id) {
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
    else if (id == 4 && money >= wall::Price)
        preview = new wall(0, 0,BLUE);
    else if (id == 5 && money >= 10) {
        Enemy *enemy;
        UnitGroups[RED]->AddNewObject(enemy=new SonicEnemy(Engine::GameEngine::GetInstance().GetMousePosition().x,
                                                     Engine::GameEngine::GetInstance().GetMousePosition().y, RED));
        enemy->UpdatePath(mapDistance);
        return;
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
bool PlayScene::CheckSpaceValid(int x, int y,Turret* building) {
	if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)return false;
    mapBuildings[y][x]=building;
	// std::vector<std::vector<int>> map = CalculateBFSDistance(0);
	// if (map[0][0] == -1) {
    //     mapDistance =  CalculateBFSDistance(1);
    //     return true;
    // }
    // mapDistance = map;
	if(mapDirection[y][x]==-1) return true;
	mapDirection= std::vector<std::vector<char>>(MapHeight, std::vector<char>(MapWidth,-1));
	for (auto& it : UnitGroups[RED]->GetObjects())if(dynamic_cast<Enemy*>(it))
		dynamic_cast<Enemy*>(it)->UpdatePath(mapDistance);
	return true;
}
std::vector<std::vector<int>> PlayScene::CalculateBFSDistance(bool ignoreBuildings) {
	// Reverse BFS to find path.
	std::vector<std::vector<int>> map(MapHeight, std::vector<int>(std::vector<int>(MapWidth, -1)));
	std::queue<Engine::Point> que;
	// Push end point.
	// BFS from end point.
	if (mapTerrain [EndGridPoint.y][EndGridPoint.x] != TILE_DIRT)return map;
	que.push(Engine::Point(EndGridPoint.x, EndGridPoint.y));
	map[EndGridPoint.y][EndGridPoint.x] = 0;
	while (!que.empty()) {
		Engine::Point p = que.front();
		que.pop();
        for(auto& dir : PlayScene::directions){
			int x = p.x + dir.x;
			int y = p.y + dir.y;
			if (x < 0 || x >= PlayScene::MapWidth|| y < 0 || y >= PlayScene::MapHeight
			|| 	mapTerrain[y][x] != TILE_DIRT || map[y][x] != -1)continue;
            if(!ignoreBuildings)if(mapBuildings[y][x])continue;
			map[y][x]	=	map[p.y][p.x] + 1;
			que.push(Engine::Point(x,y));
		}
	}
	return map;
}

std::string PlayScene::AStarPathFinding(Point start, int flag)
{
	//start = Point(2,0);
	Point end = EndGridPoint;
	std::priority_queue<PathData> Q;
	Q.push(PathData(start,0,HVal(start,end),"",1));
	std::cerr<<"pathfinding started\n";
	int b = 0;
	std::string path_str="";
	while(!Q.empty()){
		PathData state = Q.top();
		while(mapDirection[state.y][state.x]!=-1){
			int dirToken	=	(int)mapDirection[state.y][state.x];
			Point dir(PlayScene::directions[dirToken]);
			state.path.push_back(dirToken+'0');
			state.x +=dir.x;
			state.y +=dir.y;
		}
		if(state	==	end){
			path_str=state.path;
			break;
		}
		Q.pop();
		std::cerr<<"bruh * "<<state.path<<'\n';
		std::cerr<<'\t'<<state.h_val+state.g_cost<<'\n';
		std::cerr<<'\t'<<state.x<<','<<state.y<<'\n';
		for(int i=0;i<8;i++){
			if(opd[i]==state.premove)	continue;
			Point dir(directions[i]);
			Point next(state+dir);
			//std::cerr<<next.x<<','<<next.y<<'\n';
			if(next.x<0 || next.x>=MapWidth
			|| next.y<0	|| next.y>=MapHeight)
				continue;
			if(mapTerrain[next.y][next.x]!=TILE_FLOOR&& (i<4
			|| (mapTerrain[next.y][state.x]!=TILE_FLOOR&&mapTerrain[state.y][next.x]!=TILE_FLOOR))){
				int g	=	state.g_cost;
				if(i>=4){
					if(	mapTerrain[next.y][state.x]==TILE_FLOOR
					||	mapTerrain[state.y][next.x]==TILE_FLOOR)
						continue;
					g+=14;
				}
				else	g+=10;
				if(HasBuildingAt(next.x,next.y)){
					g	+=	mapBuildings[next.y][next.x]->GetHp()/(float)BlockSize * 10.0f;	
				}
				//std::cerr<<next.x<<','<<next.y<<'\n';
				PathData	N		=	PathData(next,g,HVal(next,end),state.path,i);
				N.path.push_back('0'+i);
				Q.push(N);	
				// Q.push(PathData(next,g,HVal(next,end),state.path,i));
				//std::cerr<<PathData(next,g,HVal(next,end),state.path,i).x<<','<<PathData(next,g,HVal(next,end),state.path,i).y<<'\n';
			}
		}
	}
	if(Q.empty()){
		std::cerr<<"pathfinding error\n";
    	return std::string();
	}

	std::cerr<<"pathfinding over\n"<<Q.top().path<<'\n';
	return	path_str;
}

int PlayScene::HVal(Point A, Point B)
{
	// int dx = (A.x-B.x)*10.0f;
	// int dy = (A.y-B.y)*10.0f;
	// return 2*(Point(dx
	if(A==B)	return 0;
	//if(A.x<0 || A.y<0)
	if(B==EndGridPoint){
		if(mapHValue[A.y][A.x]!=-1)
			return	mapHValue[A.y][A.x];
		
		int rtv = mapHValue[A.y][A.x]=floor((A-B).Magnitude()*10*2);
		Point D(A.x-B.x,A.y-B.y);
		Point C = A;
		for(int mul=rtv;C.x>=0&&C.x<MapWidth&&C.y>=0&&C.y<MapHeight;mul+=rtv){
			if(mapHValue[C.y][C.x]==-1){
				mapHValue[C.y][C.x]=mul;
			}
			C = C + D;
		}
		return	rtv;
	}
    return floor((A-B).Magnitude()*10*2);
	//return abs(A.x-B.x+A.y-B.y)*10;
	//else return	1;
}

void PlayScene::RemoveBuilding(int x, int y){
    std::cerr<<"start removing building\n";
	mapBuildings[y][x]=nullptr;
	if(mapTerrain[y][x]!=TILE_FLOOR){
		mapDirection= std::vector<std::vector<char>>(MapHeight, std::vector<char>(MapWidth,-1));
	// mapDistance = CalculateBFSDistance(0);
    // if(mapDistance[y][x]==-1)mapDistance = CalculateBFSDistance(1);
		for (auto& it : UnitGroups[RED]->GetObjects())if(dynamic_cast<Enemy*>(it))dynamic_cast<Enemy*>(it)->UpdatePath(mapDistance);
	}
	std::cerr<<"successfully removed building\n";
}
Turret *PlayScene::HasBuildingAt(int x, int y)
{
	if(x == EndGridPoint.x && y == EndGridPoint.y)	return	nullptr;
	return mapBuildings[y][x];
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

