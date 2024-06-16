#ifndef STAGESELECTSCENE_HPP
#define STAGESELECTSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>
#include "Engine/IScene.hpp"
#include <vector>

const int max_level_count = 10;
const int max_stage_count = 10;

class StageSelectScene final : public Engine::IScene {
private:
	std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
	int halfW;
    int halfH;
	int stageId=1;
	int levelCnt=max_level_count;
	int buttonWidth     =   400;
    int buttonHeight    =   100;
	int halfBW     		=   200;
    int halfBH    		=   50;
	int deltah  =   buttonHeight*3/2;
	int scrollY;
	int h1, h2;
	Engine::Group *LevelButtons;
public:
	explicit StageSelectScene() = default;
	void Initialize() override;
	void Terminate() override;
	void LoadLevelButtons();
	void PlayOnClick(int level);
    void ScoreboardOnClick();
	void BackOnClick(int stage);
	void ToStage(bool isToNext);
	void OnMouseScroll(int mx, int my, int delta) override;
};

#endif // STAGESELECTSCENE_HPP
