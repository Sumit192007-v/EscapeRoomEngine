#include "Puzzles.h"

#include <algorithm>

#include "Config.h"
#include "TextRenderer.h"
#include "UI.h"

namespace {
bool leftClick(const SDL_Event& e) {
    return e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT;
}
bool isEnter(const SDL_Event& e) {
    return e.type == SDL_KEYDOWN &&
           (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_KP_ENTER);
}
}  // namespace

// ---------------------------------------------------------------- RiddlePuzzle
void RiddlePuzzle::submit() {
    if (matches(input_)) succeed("Correct!");
    else fail("Not quite. Try again.");
}

void RiddlePuzzle::handleEvent(const SDL_Event& e, Context&, const SDL_Rect& area, const Item*) {
    if (solved_) return;
    SDL_Rect submitBtn{area.x + 540, area.y + 100, 160, 46};
    if (e.type == SDL_TEXTINPUT) {
        if (input_.size() < 28) input_ += e.text.text;
    } else if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_BACKSPACE && !input_.empty()) input_.pop_back();
        else if (isEnter(e)) submit();
    } else if (leftClick(e) && ui::contains(submitBtn, e.button.x, e.button.y)) {
        submit();
    }
}

void RiddlePuzzle::draw(Context& ctx, const SDL_Rect& area) {
    ctx.text->drawWrapped(question_, area.x, area.y, area.w, col::White, 22);
    SDL_Rect box{area.x, area.y + 100, 520, 46};
    ui::fillRect(ctx.renderer, box, SDL_Color{10, 10, 18, 255});
    ui::outlineRect(ctx.renderer, box, solved_ ? col::Green : col::Gold, 2);
    std::string shown = input_;
    if (!solved_ && (SDL_GetTicks() / 500) % 2 == 0) shown += "|";
    ctx.text->draw(shown, box.x + 12, box.y + 9, col::White, 24);
    ui::drawButton(ctx, SDL_Rect{area.x + 540, area.y + 100, 160, 46}, "Submit", !solved_);
    drawFeedback(ctx, area.x, area.y + 165);
}

// -------------------------------------------------------------- CodeLockPuzzle
static const char* kKeyLabels[12] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "C", "OK"};

SDL_Rect CodeLockPuzzle::keyRect(const SDL_Rect& area, int index) {
    int col = index % 6, row = index / 6;
    return SDL_Rect{area.x + col * 74, area.y + 120 + row * 60, 66, 52};
}

void CodeLockPuzzle::submit() {
    if (matches(digits_)) succeed("Click! The lock opens.");
    else {
        fail("Wrong code.");
        digits_.clear();
    }
}

void CodeLockPuzzle::pressKey(int index) {
    std::string label = kKeyLabels[index];
    if (label == "C") digits_.clear();
    else if (label == "OK") submit();
    else if (digits_.size() < answer_.size()) digits_ += label;
}

void CodeLockPuzzle::handleEvent(const SDL_Event& e, Context&, const SDL_Rect& area, const Item*) {
    if (solved_) return;
    if (e.type == SDL_TEXTINPUT) {
        char c = e.text.text[0];
        if (c >= '0' && c <= '9' && digits_.size() < answer_.size()) digits_ += c;
    } else if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_BACKSPACE && !digits_.empty()) digits_.pop_back();
        else if (isEnter(e)) submit();
    } else if (leftClick(e)) {
        for (int i = 0; i < 12; ++i)
            if (ui::contains(keyRect(area, i), e.button.x, e.button.y)) {
                pressKey(i);
                break;
            }
    }
}

void CodeLockPuzzle::draw(Context& ctx, const SDL_Rect& area) {
    ctx.text->drawWrapped(question_, area.x, area.y, area.w, col::White, 22);
    SDL_Rect disp{area.x, area.y + 55, 300, 52};
    ui::fillRect(ctx.renderer, disp, SDL_Color{10, 10, 18, 255});
    ui::outlineRect(ctx.renderer, disp, solved_ ? col::Green : col::Gold, 2);
    std::string shown;
    for (size_t i = 0; i < answer_.size(); ++i) {
        shown += (i < digits_.size()) ? std::string(1, digits_[i]) : std::string("_");
        shown += "  ";
    }
    ctx.text->draw(shown, disp.x + 18, disp.y + 8, col::White, 30);
    for (int i = 0; i < 12; ++i) ui::drawButton(ctx, keyRect(area, i), kKeyLabels[i], !solved_);
    drawFeedback(ctx, area.x + 330, area.y + 68);
}

// -------------------------------------------------------------- SequencePuzzle
namespace {
struct SeqButton { char letter; const char* name; SDL_Color color; };
const SeqButton kSeq[4] = {
    {'R', "Red",    {200, 50, 50, 255}},
    {'G', "Green",  {50, 170, 70, 255}},
    {'B', "Blue",   {50, 90, 210, 255}},
    {'Y', "Yellow", {225, 195, 40, 255}},
};
SDL_Color colorFor(char c) {
    for (const auto& b : kSeq) if (b.letter == c) return b.color;
    return SDL_Color{128, 128, 128, 255};
}
}  // namespace

SDL_Rect SequencePuzzle::buttonRect(const SDL_Rect& area, int i) {
    return SDL_Rect{area.x + i * 150, area.y + 70, 130, 70};
}
SDL_Rect SequencePuzzle::resetRect(const SDL_Rect& area) {
    return SDL_Rect{area.x + 620, area.y + 70, 120, 70};
}

void SequencePuzzle::handleEvent(const SDL_Event& e, Context&, const SDL_Rect& area, const Item*) {
    if (solved_ || !leftClick(e)) return;
    int mx = e.button.x, my = e.button.y;
    if (ui::contains(resetRect(area), mx, my)) {
        seq_.clear();
        feedback_.clear();
        return;
    }
    for (int i = 0; i < 4; ++i) {
        if (ui::contains(buttonRect(area, i), mx, my)) {
            seq_ += kSeq[i].letter;
            if (seq_.size() == answer_.size()) {
                if (matches(seq_)) succeed("The mechanism clicks open!");
                else {
                    fail("Nothing happens. The sequence resets.");
                    seq_.clear();
                }
            }
            break;
        }
    }
}

void SequencePuzzle::draw(Context& ctx, const SDL_Rect& area) {
    ctx.text->drawWrapped(question_, area.x, area.y, area.w, col::White, 22);
    for (int i = 0; i < 4; ++i) {
        SDL_Rect r = buttonRect(area, i);
        bool hover = !solved_ && ui::contains(r, ctx.mouseX, ctx.mouseY);
        SDL_Color c = kSeq[i].color;
        if (hover) { c.r = static_cast<Uint8>(std::min(255, c.r + 40)); c.g = static_cast<Uint8>(std::min(255, c.g + 40)); c.b = static_cast<Uint8>(std::min(255, c.b + 40)); }
        ui::fillRect(ctx.renderer, r, c);
        ui::outlineRect(ctx.renderer, r, hover ? col::Gold : col::White, 2);
        ctx.text->drawCentered(kSeq[i].name, r, col::White, 22);
    }
    ui::drawButton(ctx, resetRect(area), "Reset", !solved_);
    // show what has been entered so far
    ctx.text->draw("Entered:", area.x, area.y + 165, col::Grey, 20);
    for (size_t i = 0; i < seq_.size(); ++i) {
        SDL_Rect sq{area.x + 110 + static_cast<int>(i) * 44, area.y + 160, 36, 36};
        ui::fillRect(ctx.renderer, sq, colorFor(seq_[i]));
        ui::outlineRect(ctx.renderer, sq, col::White, 1);
    }
    drawFeedback(ctx, area.x, area.y + 215);
}

// ---------------------------------------------------------------- ItemUsePuzzle
SDL_Rect ItemUsePuzzle::useRect(const SDL_Rect& area) { return SDL_Rect{area.x, area.y + 110, 200, 52}; }

void ItemUsePuzzle::handleEvent(const SDL_Event& e, Context&, const SDL_Rect& area, const Item* sel) {
    if (solved_ || !leftClick(e)) return;
    if (!ui::contains(useRect(area), e.button.x, e.button.y)) return;
    if (!sel) fail("Select an item in your inventory first.");
    else if (sel->id == answer_) succeed("It works! The " + sel->name + " was used.");
    else fail("The " + sel->name + " doesn't work here.");
}

void ItemUsePuzzle::draw(Context& ctx, const SDL_Rect& area) {
    ctx.text->drawWrapped(question_, area.x, area.y, area.w, col::White, 22);
    ui::drawButton(ctx, useRect(area), "Use selected item", !solved_, 20);
    drawFeedback(ctx, area.x, area.y + 185);
}
