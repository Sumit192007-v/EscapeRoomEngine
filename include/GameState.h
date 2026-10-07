#pragma once
// GameState.h - State pattern: each screen (menu, playing, result) is a GameState.
#include <SDL.h>

class Game;

class GameState {
public:
    explicit GameState(Game& game) : game_(game) {}
    virtual ~GameState() = default;
    virtual void handleEvent(const SDL_Event& e) = 0;
    virtual void update(double dt) = 0;
    virtual void render() = 0;

protected:
    Game& game_;
};
