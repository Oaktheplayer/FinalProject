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
#include "Turret/MachineGunTurret.hpp"
#include "Turret/MissileTurret.hpp"
#include "Turret/Flamethrower.hpp"
#include "UI/Animation/Plane.hpp"
#include "Enemy/PlaneEnemy.hpp"
#include "PlayScene.hpp"
#include "Engine/Resources.hpp"
#include "Enemy/SoldierEnemy.hpp"
#include "Enemy/TankEnemy.hpp"
#include	"Enemy/TruckEnemy.hpp"
#include "Turret/TurretButton.hpp"
//#include "Engine/Unit.hpp"

#include	<iostream>

float scale;

bool PlayScene::DebugMode = false;
const std::vector<Engine::Point> PlayScene::directions = { Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1) };
const int PlayScene::MapWidth = 20, PlayScene::MapHeight = 13;
const int PlayScene::BlockSize = 64;
const float PlayScene::DangerTime = 7.61;
const Engine::Point PlayScene::SpawnGridPoint = Engine::Point(-1, 0);
const Engine::Point PlayScene::EndGridPoint = Engine::Point(MapWidth, MapHeight - 1);
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
	mapState.clear();
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
	MapComponent->AddNewObject(TowerGroup = new Group());
	MapComponent->AddNewObject(EnemyGroup = new Group());
	MapComponent->AddNewObject(BulletGroup = new Group());
	MapComponent->AddNewObject(EffectGroup = new Group());
	// Should support buttons.
	AddNewControlObject(UIGroup = new Group());
	ReadMap();
	ReadEnemyWave();
	mapDistance = CalculateBFSDistance();
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
	sight = sight + sight_dir*sight_speed;
	if (SpeedMult == 0)
		deathCountDown = -1;
	else if (deathCountDown != -1)
		SpeedMult = 1;
	// Calculate danger zone.
	std::vector<float> reachEndTimes;
	for (auto& it : EnemyGroup->GetObjects()) {
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
			if (EnemyGroup->GetObjects().empty()) {
				ScorePoint(lives^2);
				RecordScore();
				// Free resources.
				//delete TileMapGroup;
				//delete GroundEffectGroup;
				//delete DebugIndicatorGroup;
				//delete TowerGroup;
				//delete EnemyGroup;
				//delete BulletGroup;
				//delete EffectGroup;
				//delete UIGroup;
				//delete imgTarget;
				
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
		EnemyGroup->AddNewObject(enemy = new SoldierEnemy(x, y));
		break;
	case 2:
		EnemyGroup->AddNewObject(enemy = new PlaneEnemy(x, y));
		break;
	case 3:
		EnemyGroup->AddNewObject(enemy = new TankEnemy(x, y));
		break;
	case 4:
		EnemyGroup->AddNewObject(enemy = new TruckEnemy(x, y));
		break;
    // TODO: [CUSTOM-ENEMY]: You need to modify 'Resource/enemy1.txt', or 'Resource/enemy2.txt' to spawn the 4th enemy.
    //         The format is "[EnemyId] [TimeDelay] [Repeat]".
    // TODO: [CUSTOM-ENEMY]: Enable the creation of the enemy.
	default:
		return nullptr;
	}	
	enemy->UpdatePath(mapDistance);
	// enemy->UpdatePath(mapDistance);
	return enemy;
}
void PlayScene::Draw(float scale, float cx, float cy, float sx, float sy) const {
	//IScene::Draw();
	al_clear_to_color(al_map_rgb(0, 0, 0));
	MapComponent->Draw(this->scale, center.x, center.y, sight.x, sight.y);
	UIGroup->Draw();
	if(imgTarget->Visible)	imgTarget->Draw(this->scale, center.x, center.y, sight.x, sight.y);
	if(preview)				preview->Draw(this->scale,preview->Position.x,preview->Position.y,preview->Position.x,preview->Position.y);
	if (DebugMode) {
		// Draw reverse BFS distance on all reachable blocks.
		for (int i = 0; i < MapHeight; i++) {
			for (int j = 0; j < MapWidth; j++) {
				if (mapDistance[i][j] != -1) {
					// Not elegant nor efficient, but it's quite enough for debugging.
					int x = (j + 0.5) * BlockSize;
					int y = (i + 0.5) * BlockSize;
					Engine::Label label(std::to_string(mapDistance[i][j]), "pirulen.ttf", 32,
					(j + 0.5) * BlockSize,
					(i + 0.5) * BlockSize);
					label.Anchor = Engine::Point(0.5, 0.5);
					label.Draw(this->scale, center.x, center.y, sight.x, sight.y);
				}
			}
		}
	}
}
void PlayScene::OnMouseScroll(int mx, int my, int delta){
	float	pre_s	=	scale;
	scale+= (float)delta /4;
	if(scale>4)		scale=4;
	else
	if(scale<0.25)	scale=0.25;

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
//TODO: make preview appear with write scale and places turrets on the correct position
void PlayScene::OnMouseUp(int button, int mx, int my) {
	IScene::OnMouseUp(button, mx, my);
	if (!imgTarget->Visible)
		return;
	const int x = ((float)(mx-center.x)/scale + sight.x) 	/	BlockSize;
	const int y = ((float)(my-center.y)/scale + sight.y)	/	BlockSize;
	if (button & 1) {
		if (x>=0 && x<=MapWidth && y>=0 && y<=MapWidth && mapState[y][x] != TILE_OCCUPIED) {
			if (!preview)
				return;
			// Check if valid.
			if (!CheckSpaceValid(x, y)) {
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
			TowerGroup->AddNewObject(preview);
			// To keep responding when paused.
			preview->Update(0);
			// Remove Preview.
			preview = nullptr;

			mapState[y][x] = TILE_OCCUPIED;
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
	// else if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9) {
	// 	// Hotkey for Speed up.
	// 	SpeedMult = keyCode - ALLEGRO_KEY_0;
	// }
	else if(keyCode	==	ALLEGRO_KEY_UP){
		SpeedMult++;
		if(SpeedMult>10)SpeedMult=10;
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
	std::vector<bool> mapData;
	std::ifstream fin(filename);
	while (fin >> c) {
		switch (c) {
		case '0': mapData.push_back(false); break;
		case '1': mapData.push_back(true); break;
		case '\n':
		case '\r':
			if (static_cast<int>(mapData.size()) / MapWidth != 0)
				throw std::ios_base::failure("Map data is corrupted.");
			break;
		default: throw std::ios_base::failure("Map data is corrupted.");
		}
	}
	fin.close();
	// Validate map data.
	if (static_cast<int>(mapData.size()) != MapWidth * MapHeight)
		throw std::ios_base::failure("Map data is corrupted.");
	// Store map in 2d array.
	mapState = std::vector<std::vector<TileType>>(MapHeight, std::vector<TileType>(MapWidth));
	for (int i = 0; i < MapHeight; i++) {
		for (int j = 0; j < MapWidth; j++) {
			const int num = mapData[i * MapWidth + j];
			mapState[i][j] = num ? TILE_FLOOR : TILE_DIRT;
			//fprintf(stderr,"%d ",mapState[i][j]);
			if (num)
				TileMapGroup->AddNewObject(new Engine::Image("play/floor.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
			else
				TileMapGroup->AddNewObject(new Engine::Image("play/dirt.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
		}
		//fprintf(stderr,"\n");
	}
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
	btn = new TurretButton("play/floor.png", "play/dirt.png",
		Engine::Sprite("play/tower-base.png", x+i%4*dx, y, 0, 0, 0, 0),
		Engine::Sprite("play/turret-1.png", x+i%4*dx, y - 8, 0, 0, 0, 0)
		, x+i%4*dx, y, MachineGunTurret::Price);
	// Reference: Class Member Function Pointer and std::bind.
	btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 0));
	UIGroup->AddNewControlObject(btn);
	i++;
	// Button 2
	btn = new TurretButton("play/floor.png", "play/dirt.png",
		Engine::Sprite("play/tower-base.png", x+i%4*dx, y, 0, 0, 0, 0),
		Engine::Sprite("play/turret-2.png", x+i%4*dx, y - 8, 0, 0, 0, 0)
		, x+i%4*dx, y, LaserTurret::Price);
	btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 1));
	UIGroup->AddNewControlObject(btn);
	i++;
	// Button 3
	btn = new TurretButton("play/floor.png", "play/dirt.png",
		Engine::Sprite("play/tower-base.png", 1446, y, 0, 0, 0, 0),
		Engine::Sprite("play/turret-3.png", 1446, y - 8, 0, 0, 0, 0)
		, x+i%4*dx, y, MissileTurret::Price);
	btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 2));
	UIGroup->AddNewControlObject(btn);
	i++;

	btn = new TurretButton("play/floor.png", "play/dirt.png",
		Engine::Sprite("play/tower-base.png", x+i%4*dx, y, 0, 0, 0, 0),
		Engine::Sprite("play/turret-6.png", x+i%4*dx, y - 8, 0, 0, 0, 0)
		, x+i%4*dx, y, Flamethrower::Price);
	btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 3));
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
		preview = new MachineGunTurret(0, 0);
	else if (id == 1 && money >= LaserTurret::Price)
		preview = new LaserTurret(0, 0);
	else if (id == 2 && money >= MissileTurret::Price)
		preview = new MissileTurret(0, 0);
	else if (id == 3 && money >= Flamethrower::Price)
		preview = new Flamethrower(0, 0);
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

bool PlayScene::CheckSpaceValid(int x, int y) {
	if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
		return false;
	auto map00 = mapState[y][x];
	mapState[y][x] = TILE_OCCUPIED;
	std::vector<std::vector<int>> map = CalculateBFSDistance();
	mapState[y][x] = map00;
	if (map[0][0] == -1)
		return false;
	for (auto& it : EnemyGroup->GetObjects()) {
		Engine::Point pnt;
		pnt.x = floor(it->Position.x / BlockSize);
		pnt.y = floor(it->Position.y / BlockSize);
		if (pnt.x < 0) pnt.x = 0;
		if (pnt.x >= MapWidth) pnt.x = MapWidth - 1;
		if (pnt.y < 0) pnt.y = 0;
		if (pnt.y >= MapHeight) pnt.y = MapHeight - 1;
		if (map[pnt.y][pnt.x] == -1)
			return false;
	}
	// All enemy have path to exit.
	mapState[y][x] = TILE_OCCUPIED;
	mapDistance = map;
	for (auto& it : EnemyGroup->GetObjects())
		dynamic_cast<Enemy*>(it)->UpdatePath(mapDistance);
	return true;
}
std::vector<std::vector<int>> PlayScene::CalculateBFSDistance() {
	// Reverse BFS to find path.
	std::vector<std::vector<int>> map(MapHeight, std::vector<int>(std::vector<int>(MapWidth, -1)));
	std::queue<Engine::Point> que;
	// Push end point.
	// BFS from end point.
	if (mapState[MapHeight - 1][MapWidth - 1] != TILE_DIRT)
		return map;
	que.push(Engine::Point(MapWidth - 1, MapHeight - 1));
	map[MapHeight - 1][MapWidth - 1] = 0;
	while (!que.empty()) {
		Engine::Point p = que.front();
		que.pop();
		// DONE: [BFS PathFinding] (1/1): Implement a BFS starting from the most right-bottom block in the map.
		//               For each step you should assign the corresponding distance to the most right-bottom block.
		//               mapState[y][x] is TILE_DIRT if it is empty.
		for(auto& dir : PlayScene::directions){
			int x = p.x + dir.x;
			int y = p.y + dir.y;
			if (x < 0 || x >= PlayScene::MapWidth || y < 0 || y >= PlayScene::MapHeight
			|| 	mapState[y][x] != TILE_DIRT || map[y][x] != -1)
				continue;
			map[y][x]	=	map[p.y][p.x] + 1;
			que.push(Engine::Point(x,y));
		}
	}
	return map;
}

void PlayScene::ScorePoint(int point){
	score+=point;
	UIScore->Text	=	"Score "+std::to_string(score);
}

void PlayScene::RecordScore()
{
    std::ofstream   fout("Resource/currentdata.txt");
    fout << score;
}
