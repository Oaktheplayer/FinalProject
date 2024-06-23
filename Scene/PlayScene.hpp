#ifndef PLAYSCENE_HPP
#define PLAYSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <list>
#include <queue>
#include <memory>
#include <utility>
#include <vector>

#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/Unit.hpp"
#include "Building/Building.hpp"
// #include "Enemy/Enemy.hpp"

class Enemy;
class Turret;
class Building;
namespace Engine {
	class Group;
	class Image;
	class Label;
	class Sprite;
}  // namespace Engine




//const Point directions[] = {Point(1,0),Point(-1,0),Point(0,1),Point(0,-1)};

class PlayScene  : public Engine::IScene {
private:

	ALLEGRO_SAMPLE_ID bgmId;
	std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> deathBGMInstance;
protected:
	int lives;
	int money;
	int SpeedMult;
	enum TileType {
        TILE_DIRT,
        TILE_FLOOR,
        TILE_WATER
    };
public:
	static bool DebugMode;
	static const std::vector<Engine::Point> directions;
	int MapWidth, MapHeight;
	static const int BlockSize;
	static const float DangerTime;
	Engine::Point SpawnGridPoint;
	Engine::Point EndGridPoint;
	static const std::vector<int> code;
	int StageId=0;
	int MapId;
	///Defend: 0
	///Attack: 1
    int gamemode;
	float ticks;
	float deathCountDown;
	float scale;
	Engine::Point center;
	Engine::Point sight;
	Engine::Point sight0;
	Engine::Point sight_dir;
	float	sight_speed;
    bool BaseExsist;
    ALLEGRO_TIMER* timer;
	// For everything on the map (non-UI)
	Group* MapComponent;
	// Map tiles.
	Group* TileMapGroup;
	Group* GroundEffectGroup;
	Group* DebugIndicatorGroup;
	Group* UnitGroups[TEAM_COUNT];
	Group* BulletGroup;
	// Group* TowerGroup;
	// Group* EnemyGroup;
	Group* EffectGroup;
	Group* UIGroup;
	Engine::Label* UIMoney;
	Engine::Label* UILives;
	Engine::Label* UIScore;
	Engine::Image* imgTarget;
	Engine::Sprite* dangerIndicator;
	Unit* preview;
	std::vector<std::vector<TileType>> mapTerrain;
	std::vector<std::vector<int>> mapDistance;
	std::vector<std::vector<int>> mapHValue;
	std::vector<std::vector<int>> mapGCost;
	std::vector<std::vector<bool>> mapAStarVisited;
	std::vector<std::vector<char>> mapDirection;
	//TEST
	//std::vector<std::vector<Turret*>> mapBuildings;
	std::vector<std::vector<Building*>> mapBuildings;
	std::list<std::pair<int, float>> enemyWaveData;
	std::list<int> keyStrokes;
	Engine::Point GetClientSize();
	explicit PlayScene() = default;
	void Initialize() override;
	void Terminate() override;
	void Update(float deltaTime) override;
	Enemy* SpawnEnemy(int type, float x, float y, float delta=0);
	void Draw(float scale=1, float cx=0, float cy=0, float sx=0, float sy=0) const override;
	void OnMouseScroll(int mx, int my, int delta) override;
	void OnMouseDown(int button, int mx, int my) override;
	void OnMouseMove(int mx, int my) override;
	void OnMouseUp(int button, int mx, int my) override;
	void OnKeyDown(int keyCode) override;
	void OnKeyUp(int keyCode) override;
	void Hit();
	int GetMoney() const;
	void EarnMoney(int money);
	void ReadMap();
	void ReadEnemyWave();
	void ConstructUI();
    void AttackModeUI();
    void DefenceModeUI();
	void DMUIBtnClicked(int id);
    void AMUIBtnClicked(int id);
    bool CheckSpaceValid(int x, int y,Unit* building);
	//std::vector<std::vector<int>> CalculateBFSDistance(bool);
	std::string	AStarPathFinding(Engine::Point start, int flag=0);
	bool CanPassGridPoint(int x, int y, int flag=0, bool ignoreBuilding=false);
	int	HVal(Engine::Point A, Engine::Point B);
	void RemoveBuilding(int x,int y);
	Building* HasBuildingAt(int x,int y);
	Building* HasBuildingAt(Engine::Point p);
	int score;
	void ScorePoint(int x);
	void RecordScore();
	// void ModifyReadMapTiles();
};

class PathData: public Engine::Point{
	private:
	public:
		int g_cost,h_val;
		int premove;
		std::string	path;
		bool repathing;
		PathData(Engine::Point p, int g, int h, std::string P, int pre, bool repath = false): Engine::Point(p),g_cost(g),h_val(h),path(std::string(P)),premove(pre),repathing(repath){
		}
		PathData(const PathData &P):
			Engine::Point((Engine::Point)P),
			g_cost(P.g_cost),
			h_val(P.h_val),
			path(P.path),
			premove(P.premove),
			repathing(P.repathing){
		}
		auto operator<(const PathData &P) const{
			if(g_cost+h_val==P.g_cost+P.h_val){
                if(h_val!=P.h_val)return h_val>P.h_val;
                return	repathing;
            }
			return g_cost+h_val>P.g_cost+P.h_val;
		}

};

#endif // PLAYSCENE_HPP
