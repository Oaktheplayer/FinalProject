#ifndef ScoreboardScene_HPP
#define ScoreboardScene_HPP

#include <allegro5/allegro_audio.h>
#include <memory>
#include "Engine/IScene.hpp"
class   Record{
    public:
        std::string username;
        int         score;
    public:
        explicit    Record(){
            username    =   "";
            score       =   0;
        }
        Record(std::string, int);
        // friend  std::istream &operator>>(std::istream,Record);
};

class ScoreboardScene final : public Engine::IScene {
private:
    int page;
    int maxPage;
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    std::vector<Record> record;
    int                 currentLength;
    Engine::Label*      usernames[10];
    Engine::Label*      scores[10];
    // Engine::ImageButton*    nextPageBtn;
    // Engine::ImageButton*    prevPageBtn;    
public:
    explicit ScoreboardScene() = default;

    void Initialize() override;

    void Terminate() override;

    void ReadScoreboard();

    void LoadScoreBoard(int page);

    void ToPage(bool isToNext);

    void BackOnClick(int stage);

    void ClearScoreBoard();
};




#endif // ScoreboardScene_HPP
