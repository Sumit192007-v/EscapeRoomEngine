#include "DialLockPuzzle.h"

#include "Config.h"
#include "TextRenderer.h"
#include "UI.h"

DialLockPuzzle::DialLockPuzzle(const std::string& question, const std::string& answer)
    : Puzzle(question, answer), digits_(answer.size(), '0') {}

SDL_Rect DialLockPuzzle::upRect(const SDL_Rect& a, int i)    { return SDL_Rect{a.x + i * 90, a.y + 60, 70, 36}; }
SDL_Rect DialLockPuzzle::digitRect(const SDL_Rect& a, int i) { return SDL_Rect{a.x + i * 90, a.y + 100, 70, 70}; }
SDL_Rect DialLockPuzzle::downRect(const SDL_Rect& a, int i)  { return SDL_Rect{a.x + i * 90, a.y + 174, 70, 36}; }
SDL_Rect DialLockPuzzle::openRect(const SDL_Rect& a)         { return SDL_Rect{a.x + 520, a.y + 100, 160, 70}; }

void DialLockPuzzle::turn(int dial, int delta) {
    char& c = digits_[dial];
    c = static_cast<char>('0' + ((c - '0') + delta + 10) % 10);  // wraps 9 -> 0 and 0 -> 9
}

void DialLockPuzzle::handleEvent(const SDL_Event& e, Context&, const SDL_Rect& area, const Item*) {
    if (solved_) return;
    if (e.type != SDL_MOUSEBUTTONDOWN || e.button.button != SDL_BUTTON_LEFT) return;
    int mx = e.button.x, my = e.button.y;

    for (int i = 0; i < static_cast<int>(digits_.size()); ++i) {
        if (ui::contains(upRect(area, i), mx, my))   { turn(i, +1); return; }
        if (ui::contains(downRect(area, i), mx, my)) { turn(i, -1); return; }
    }
    if (ui::contains(openRect(area), mx, my)) {
        if (matches(digits_)) succeed("Click! The dials line up and the lock opens.");
        else fail("The lock stays shut.");
    }
}

void DialLockPuzzle::draw(Context& ctx, const SDL_Rect& area) {
    ctx.text->drawWrapped(question_, area.x, area.y, area.w, col::White, 22);
    for (int i = 0; i < static_cast<int>(digits_.size()); ++i) {
        ui::drawButton(ctx, upRect(area, i), "^", !solved_);
        SDL_Rect d = digitRect(area, i);
        ui::fillRect(ctx.renderer, d, SDL_Color{10, 10, 18, 255});
        ui::outlineRect(ctx.renderer, d, solved_ ? col::Green : col::Gold, 2);
        ctx.text->drawCentered(std::string(1, digits_[i]), d, col::White, 40);
        ui::drawButton(ctx, downRect(area, i), "v", !solved_);
    }
    ui::drawButton(ctx, openRect(area), "Open", !solved_);
    drawFeedback(ctx, area.x, area.y + 225);
}
