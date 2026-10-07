#pragma once
#include "GameState.h"

class MenuState : public GameState {
public:
    explicit MenuState(Game& game);
    void handleEvent(const SDL_Event& e) override;
    void update(double) override {}
    void render() override;

private:
    bool hasSave_;
};
