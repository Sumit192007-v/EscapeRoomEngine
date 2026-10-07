#pragma once
#include <string>

#include "GameState.h"

// Shown after a level: Level Complete, Game Over, or the final Win screen.
class ResultState : public GameState {
public:
    enum class Kind { LevelComplete, GameOver, Win };
    ResultState(Game& game, Kind kind, int levelIndex, std::string title, std::string subtitle);
    void handleEvent(const SDL_Event& e) override;
    void update(double) override {}
    void render() override;

private:
    void primaryAction();
    Kind kind_;
    int levelIndex_;
    std::string title_, subtitle_;
};
