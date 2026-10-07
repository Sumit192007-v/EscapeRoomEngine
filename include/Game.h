#pragma once
// Game.h - owns the window/renderer/services, runs the main loop and switches GameStates.
#include <memory>
#include <string>
#include <vector>

#include "Context.h"
#include "GameState.h"
#include "SDLWrappers.h"
#include "SaveManager.h"
#include "TextRenderer.h"
#include "TextureManager.h"

class Game {
public:
    Game();
    void run();

    // The new state becomes active at a safe moment (never while the old one is mid-event).
    void changeState(std::unique_ptr<GameState> s) { pending_ = std::move(s); }
    void quit() { running_ = false; }

    Context& ctx() { return ctx_; }
    int levelCount() const { return static_cast<int>(levelFiles_.size()); }
    const std::vector<std::string>& levelFiles() const { return levelFiles_; }

    void showMenu();
    void startNewGame();
    void continueGame();
    void startLevel(int index, const SaveData* resume = nullptr);

    int score() const { return score_; }
    void addScore(int s) { score_ += s; }
    void setScore(int s) { score_ = s; }
    void showError(const std::string& msg);

private:
    // order matters: destroyed in reverse order
    SdlSystem sdl_;
    WindowPtr window_;
    RendererPtr renderer_;
    std::unique_ptr<TextureManager> textures_;
    std::unique_ptr<TextRenderer> text_;
    Context ctx_;
    std::vector<std::string> levelFiles_;
    std::unique_ptr<GameState> state_;
    std::unique_ptr<GameState> pending_;
    bool running_ = true;
    int score_ = 0;
};
