#ifndef WINSCENE_HPP
#define WINSCENE_HPP
#include <allegro5/allegro_audio.h>
#include	<ctime>
#include "Engine/IScene.hpp"


class WinScene final : public Engine::IScene {
private:
	float ticks;
	ALLEGRO_SAMPLE_ID bgmId;
	int score;
	struct	tm	*current_time;
	int date, month, year;
	std::string	username;
	std::string	to_month[12]	=	{"JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC"};
public:
	explicit WinScene() = default;
	void Initialize() override;
	void Terminate() override;
	void Update(float deltaTime) override;
	void BackOnClick(int stage);
	void ReadCurrentData();
	void RecordScore();
};

#endif // WINSCENE_HPP
