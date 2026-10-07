#include "PlayState.h"

#include <cstdio>

#include "Config.h"
#include "Game.h"
#include "LevelLoader.h"
#include "MenuState.h"
#include "ResultState.h"
#include "UI.h"

namespace {
const SDL_Rect kCloseBtn{990, 50, 40, 40};
const SDL_Rect kImageRect{260, 100, 240, 180};
const SDL_Rect kPuzzleArea{260, 310, 760, 270};

std::string formatTime(double t) {
    int s = t < 0 ? 0 : static_cast<int>(t);
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%02d:%02d", s / 60, s % 60);
    return buf;
}
}  // namespace

PlayState::PlayState(Game& game, int levelIndex, const SaveData* resume)
    : GameState(game), levelIndex_(levelIndex) {
    room_ = LevelLoader::load(game.levelFiles().at(levelIndex));  // may throw LevelParseException
    room_->attach(this);
    timeLeft_ = room_->timeLimit();
    if (resume) {
        if (resume->timeLeft > 0) timeLeft_ = resume->timeLeft;
        for (const auto& it : resume->items) inventory_.add(it);
        room_->restore(resume->completed, resume->unlocked);
    }
}

// ------------------------------------------------------------------ observer callbacks
void PlayState::onObjectUnlocked(const std::string&, const std::string& name) { toast("Something changed: " + name + " is now available!"); }
void PlayState::onItemFound(const Item& item) { toast("You found: " + item.name); }
void PlayState::onRoomEscaped() {
    escaped_ = true;
    escapeDelay_ = 2.0;
    toast("The door opens... you escaped!");
}

// ------------------------------------------------------------------ helpers
void PlayState::toast(const std::string& msg) {
    toast_ = msg;
    toastTimer_ = 3.0;
}

void PlayState::saveProgress() {
    SaveData d;
    d.levelIndex = levelIndex_;
    d.timeLeft = timeLeft_;
    d.score = game_.score();
    d.completed = room_->completedIds();
    d.unlocked = room_->unlockedIds();
    d.items = inventory_.items();
    try {
        SaveManager::save(d, cfg::kSavePath);
    } catch (const SaveFileException& e) {
        toast(e.what());
    }
}

void PlayState::openInspect(GameObject* obj) {
    inspected_ = obj;
    mode_ = Mode::Inspecting;
    if (!obj->isLocked() && !obj->puzzle()) room_->complete(*obj, inventory_);  // plain objects give their reward when looked at
}

void PlayState::closeInspect() {
    inspected_ = nullptr;
    mode_ = Mode::Exploring;
}

void PlayState::onPuzzleSolved() {
    Puzzle* p = inspected_->puzzle();
    std::string used = p->consumedItemId();
    if (!used.empty()) inventory_.remove(used);
    room_->complete(*inspected_, inventory_);
}

void PlayState::finishLevel() {
    game_.addScore(500 + static_cast<int>(timeLeft_) * 5);
    const int next = levelIndex_ + 1;
    const std::string sub = "Score: " + std::to_string(game_.score());
    if (next < game_.levelCount()) {
        SaveData d;  // auto-save: the next level starts fresh
        d.levelIndex = next;
        d.score = game_.score();
        try { SaveManager::save(d, cfg::kSavePath); } catch (const SaveFileException&) {}
        game_.changeState(std::make_unique<ResultState>(game_, ResultState::Kind::LevelComplete, levelIndex_, "Level Complete!", sub));
    } else {
        SaveManager::remove(cfg::kSavePath);
        game_.changeState(std::make_unique<ResultState>(game_, ResultState::Kind::Win, levelIndex_, "YOU ESCAPED EVERYTHING!", "Final " + sub));
    }
}

// ------------------------------------------------------------------ events
void PlayState::handleEvent(const SDL_Event& e) {
    if (escaped_) return;

    if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_F1) { GameObject::showHotspots = !GameObject::showHotspots; return; }
        if (e.key.keysym.sym == SDLK_ESCAPE) {
            if (mode_ == Mode::Inspecting) closeInspect();
            else { saveProgress(); game_.showMenu(); }
            return;
        }
        if (e.key.keysym.sym == SDLK_F5) { saveProgress(); toast("Game saved."); return; }
    }

    const bool leftClick = e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT;

    if (mode_ == Mode::Exploring) {
        if (!leftClick) return;
        if (inventory_.handleClick(e.button.x, e.button.y)) return;
        if (GameObject* o = room_->objectAt(e.button.x, e.button.y)) openInspect(o);
        return;
    }

    // Inspecting
    if (leftClick) {
        if (ui::contains(kCloseBtn, e.button.x, e.button.y)) { closeInspect(); return; }
        if (inventory_.handleClick(e.button.x, e.button.y)) return;
    }
    Puzzle* p = inspected_ ? inspected_->puzzle() : nullptr;
    if (p && !inspected_->isLocked() && !p->isSolved()) {
        p->handleEvent(e, game_.ctx(), kPuzzleArea, inventory_.selectedItem());
        if (p->isSolved()) onPuzzleSolved();
    }
}

// ------------------------------------------------------------------ update
void PlayState::update(double dt) {
    if (toastTimer_ > 0) toastTimer_ -= dt;
    if (escaped_) {
        escapeDelay_ -= dt;
        if (escapeDelay_ <= 0) finishLevel();
        return;
    }
    timeLeft_ -= dt;
    if (timeLeft_ <= 0) {
        game_.changeState(std::make_unique<ResultState>(game_, ResultState::Kind::GameOver, levelIndex_, "TIME'S UP!", "The room keeps you... try again."));
    }
}

// ------------------------------------------------------------------ drawing
void PlayState::drawHud() {
    Context& ctx = game_.ctx();
    SDL_Rect pill{10, 6, 170, 34};
    ui::fillRect(ctx.renderer, pill, SDL_Color{0, 0, 0, 170});
    ctx.text->drawCentered("Time " + formatTime(timeLeft_), pill, timeLeft_ < 30 ? col::Red : col::White, 22);
    ctx.text->draw(room_->name() + "  (Level " + std::to_string(levelIndex_ + 1) + "/" + std::to_string(game_.levelCount()) + ")", 200, 12, col::Gold, 22);
    ctx.text->draw("ESC: back/menu   F5: save", 1010, 14, col::Grey, 18);

    if (toastTimer_ > 0 && !toast_.empty()) {
        int w = ctx.text->measureWidth(toast_, 22) + 40;
        SDL_Rect box{(cfg::kScreenW - w) / 2, 580, w, 34};
        ui::fillRect(ctx.renderer, box, SDL_Color{0, 0, 0, 215});
        ui::outlineRect(ctx.renderer, box, col::Gold, 1);
        ctx.text->drawCentered(toast_, box, col::Gold, 22);
    }
}

void PlayState::drawInspect() {
    if (!inspected_) return;
    Context& ctx = game_.ctx();
    ui::fillRect(ctx.renderer, SDL_Rect{0, 0, cfg::kScreenW, cfg::kInventoryBar.y}, SDL_Color{0, 0, 0, 150});
    ui::fillRect(ctx.renderer, cfg::kInspectPanel, col::Panel);
    ui::outlineRect(ctx.renderer, cfg::kInspectPanel, col::Gold, 2);

    ctx.text->draw(inspected_->name(), 260, 55, col::Gold, 32);
    ui::drawButton(ctx, kCloseBtn, "X");

    const std::string& img = inspected_->inspectImagePath().empty() ? inspected_->imagePath() : inspected_->inspectImagePath();
    ui::drawImageOrPlaceholder(ctx, img, kImageRect, inspected_->name(), SDL_Color{70, 90, 130, 255});
    ui::outlineRect(ctx.renderer, kImageRect, col::Grey, 1);

    if (inspected_->isLocked()) {
        ctx.text->drawWrapped(inspected_->lockedMessage(), 520, 100, 490, col::Red, 22);
        return;
    }
    ctx.text->drawWrapped(inspected_->description(), 520, 100, 490, col::White, 22);

    if (Puzzle* p = inspected_->puzzle()) {
        if (p->isSolved()) ctx.text->draw("Solved!  (close this window with X or ESC)", kPuzzleArea.x, kPuzzleArea.y, col::Green, 24);
        else p->draw(ctx, kPuzzleArea);
    }
}

void PlayState::render() {
    Context& ctx = game_.ctx();
    room_->draw(ctx);
    if (mode_ == Mode::Inspecting) drawInspect();
    inventory_.draw(ctx);
    drawHud();
}
