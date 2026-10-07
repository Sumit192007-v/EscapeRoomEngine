#include "ResultState.h"

#include "Config.h"
#include "Game.h"
#include "UI.h"

namespace {
const SDL_Rect kPrimary{490, 400, 300, 60};
const SDL_Rect kSecondary{490, 480, 300, 60};
}  // namespace

ResultState::ResultState(Game& game, Kind kind, int levelIndex, std::string title, std::string subtitle)
    : GameState(game), kind_(kind), levelIndex_(levelIndex), title_(std::move(title)), subtitle_(std::move(subtitle)) {}

void ResultState::primaryAction() {
    switch (kind_) {
        case Kind::LevelComplete: game_.startLevel(levelIndex_ + 1); break;
        case Kind::GameOver:      game_.startLevel(levelIndex_); break;
        case Kind::Win:           game_.showMenu(); break;
    }
}

void ResultState::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_KEYDOWN && (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_SPACE)) {
        primaryAction();
        return;
    }
    if (e.type != SDL_MOUSEBUTTONDOWN || e.button.button != SDL_BUTTON_LEFT) return;
    if (ui::contains(kPrimary, e.button.x, e.button.y)) primaryAction();
    else if (kind_ != Kind::Win && ui::contains(kSecondary, e.button.x, e.button.y)) game_.showMenu();
}

void ResultState::render() {
    Context& ctx = game_.ctx();
    ui::fillRect(ctx.renderer, SDL_Rect{0, 0, cfg::kScreenW, cfg::kScreenH}, col::Dark);
    SDL_Color titleColor = kind_ == Kind::GameOver ? col::Red : col::Green;
    ctx.text->drawCentered(title_, SDL_Rect{0, 190, 1280, 80}, titleColor, 56);
    ctx.text->drawCentered(subtitle_, SDL_Rect{0, 290, 1280, 40}, col::White, 28);

    const char* primary = kind_ == Kind::LevelComplete ? "Next Level" : (kind_ == Kind::GameOver ? "Retry" : "Main Menu");
    ui::drawButton(ctx, kPrimary, primary);
    if (kind_ != Kind::Win) ui::drawButton(ctx, kSecondary, "Main Menu");
}
