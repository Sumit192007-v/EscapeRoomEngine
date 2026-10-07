#include "Game.h"

#include <algorithm>
#include <iostream>

#include "Config.h"
#include "LevelLoader.h"
#include "MenuState.h"
#include "PlayState.h"

Game::Game() {
    window_.reset(SDL_CreateWindow(cfg::kTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                   cfg::kScreenW, cfg::kScreenH, SDL_WINDOW_RESIZABLE));
    if (!window_) throw GameException(std::string("cannot create window: ") + SDL_GetError());

    renderer_.reset(SDL_CreateRenderer(window_.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
    if (!renderer_) renderer_.reset(SDL_CreateRenderer(window_.get(), -1, SDL_RENDERER_SOFTWARE));
    if (!renderer_) throw GameException(std::string("cannot create renderer: ") + SDL_GetError());

    SDL_RenderSetLogicalSize(renderer_.get(), cfg::kScreenW, cfg::kScreenH);
    SDL_SetRenderDrawBlendMode(renderer_.get(), SDL_BLENDMODE_BLEND);

    textures_ = std::make_unique<TextureManager>(renderer_.get());
    text_ = std::make_unique<TextRenderer>(renderer_.get(), cfg::kFontPath);
    ctx_.renderer = renderer_.get();
    ctx_.textures = textures_.get();
    ctx_.text = text_.get();

    levelFiles_ = LevelLoader::loadLevelList(cfg::kLevelList);
    SDL_StartTextInput();  // lets riddle/code puzzles receive typed characters
    state_ = std::make_unique<MenuState>(*this);
}

void Game::showError(const std::string& msg) {
    std::cerr << "[Error] " << msg << "\n";
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Escape Room Engine", msg.c_str(), window_.get());
}

void Game::showMenu() { changeState(std::make_unique<MenuState>(*this)); }

void Game::startNewGame() {
    score_ = 0;
    startLevel(0, nullptr);
}

void Game::continueGame() {
    try {
        SaveData d = SaveManager::load(cfg::kSavePath);
        if (d.levelIndex < 0 || d.levelIndex >= levelCount()) d.levelIndex = 0;
        score_ = d.score;
        startLevel(d.levelIndex, &d);
    } catch (const GameException& e) {
        showError(e.what());
    }
}

void Game::startLevel(int index, const SaveData* resume) {
    try {
        changeState(std::make_unique<PlayState>(*this, index, resume));
    } catch (const GameException& e) {  // bad level file: stay on the menu instead of crashing
        showError(e.what());
    }
}

void Game::run() {
    const double freq = static_cast<double>(SDL_GetPerformanceFrequency());
    Uint64 last = SDL_GetPerformanceCounter();

    while (running_) {
        if (pending_) state_ = std::move(pending_);

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) { running_ = false; break; }
            if (e.type == SDL_MOUSEMOTION) { ctx_.mouseX = e.motion.x; ctx_.mouseY = e.motion.y; }
            else if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) { ctx_.mouseX = e.button.x; ctx_.mouseY = e.button.y; }
            if (state_) state_->handleEvent(e);
            if (pending_) state_ = std::move(pending_);
        }

        Uint64 now = SDL_GetPerformanceCounter();
        double dt = std::min(0.1, static_cast<double>(now - last) / freq);
        last = now;

        if (state_) state_->update(dt);

        SDL_SetRenderDrawColor(renderer_.get(), 0, 0, 0, 255);
        SDL_RenderClear(renderer_.get());
        if (state_) state_->render();
        SDL_RenderPresent(renderer_.get());
    }
}
