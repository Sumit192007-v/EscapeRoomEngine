#include "MenuState.h"

#include "Config.h"
#include "Game.h"
#include "UI.h"

namespace {
const SDL_Rect kNew{490, 300, 300, 60};
const SDL_Rect kContinue{490, 380, 300, 60};
const SDL_Rect kExit{490, 460, 300, 60};
}  // namespace

MenuState::MenuState(Game& game) : GameState(game), hasSave_(SaveManager::exists(cfg::kSavePath)) {}

void MenuState::handleEvent(const SDL_Event& e) {
    if (e.type != SDL_MOUSEBUTTONDOWN || e.button.button != SDL_BUTTON_LEFT) return;
    int x = e.button.x, y = e.button.y;
    if (ui::contains(kNew, x, y)) game_.startNewGame();
    else if (hasSave_ && ui::contains(kContinue, x, y)) game_.continueGame();
    else if (ui::contains(kExit, x, y)) game_.quit();
}

void MenuState::render() {
    Context& ctx = game_.ctx();
    SDL_Rect full{0, 0, cfg::kScreenW, cfg::kScreenH};
    ui::drawImageOrPlaceholder(ctx, cfg::kMenuBg, full, "", SDL_Color{20, 22, 38, 255});
    ui::fillRect(ctx.renderer, SDL_Rect{340, 120, 600, 120}, SDL_Color{0, 0, 0, 150});
    ctx.text->drawCentered("ESCAPE ROOM ENGINE", SDL_Rect{340, 130, 600, 60}, col::Gold, 48);
    ctx.text->drawCentered("Inspect. Solve. Escape.", SDL_Rect{340, 190, 600, 40}, col::White, 24);

    ui::drawButton(ctx, kNew, "New Game");
    ui::drawButton(ctx, kContinue, "Continue", hasSave_);
    ui::drawButton(ctx, kExit, "Exit");
    ctx.text->drawCentered("Click objects to inspect them  |  ESC = back/menu  |  F5 = save", SDL_Rect{0, 660, 1280, 30}, col::Grey, 18);
}
