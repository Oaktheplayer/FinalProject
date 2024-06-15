#ifndef PLAYSCENE_HPP
#define PLAYSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <list>
#include <memory>
#include <utility>
#include <vector>

#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
// #include "Enemy/Enemy.hpp"
class Enemy;
class Turret;
namespace Engine {
	class Group;
	class Image;
	class Label;
	class Sprite;
}  // namespace Engine

typedef enum effects{
	BURN,
	STATUS_EFFECT_LENGTH
}StatusEffect;
enum Team{
	BLUE,
	RED,
	TEAM_COUNT
};
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
    enum BuildingType{
        NON,
        TURRET,
        WALL,
        TOWNHALL
    };
    enum TileStat {
        TILE_EMPTY,
        TILE_OCCUPIED
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
	int MapId;
	float ticks;
	float deathCountDown;
	float	scale;
	Engine::Point center;
	Engine::Point sight;
	Engine::Point sight0;
	Engine::Point sight_dir;
	float	sight_speed;

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
	Turret* preview;
	std::vector<std::vector<TileType>> mapTerrain;
	std::vector<std::vector<int>> mapDistance;
	std::vector<std::vector<Turret*>> mapBuildings;
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
	virtual void ConstructUI();
	virtual void UIBtnClicked(int id);
	virtual bool CheckSpaceValid(int x, int y,Turret* building);
	std::vector<std::vector<int>> CalculateBFSDistance(bool);
	void RemoveBuilding(int x,int y);
	Turret* HasBuildingAt(int x,int y);
	int score;
	void 	ScorePoint(int x);
	void 	RecordScore();
	// void ModifyReadMapTiles();
};

//class DefenceMode :public PlayScene{
//    void ConstructUI() override;
//    bool CheckSpaceValid(int x, int y,Turret* building) override;
//    void UIBtnClicked(int id) override;
//};
//class AttackMode :public PlayScene{
//    void ConstructUI() override;
//    bool CheckSpaceValid(int x, int y,Turret* building) override;
//    void UIBtnClicked(int id) override;
//};
#endif // PLAYSCENE_HPP
