#pragma once
// PlayState.h - the actual gameplay screen: explore the room, inspect objects, solve puzzles.
#include <memory>
#include <string>

#include "GameState.h"
#include "Inventory.h"
#include "Observer.h"
#include "Room.h"
#include "SaveManager.h"

class PlayState : public GameState, public RoomObserver {
public:
    // resume != nullptr -> continue from a save file
    PlayState(Game& game, int levelIndex, const SaveData* resume);

    void handleEvent(const SDL_Event& e) override;
    void update(double dt) override;
    void render() override;

    // RoomObserver (Observer pattern)
    void onObjectUnlocked(const std::string& objectId, const std::string& objectName) override;
    void onItemFound(const Item& item) override;
    void onRoomEscaped() override;

private:
    enum class Mode { Exploring, Inspecting };

    void openInspect(GameObject* obj);
    void closeInspect();
    void onPuzzleSolved();
    void saveProgress();
    void toast(const std::string& msg);
    void finishLevel();
    void drawHud();
    void drawInspect();

    int levelIndex_;
    std::unique_ptr<Room> room_;
    Inventory inventory_;
    double timeLeft_;
    Mode mode_ = Mode::Exploring;
    GameObject* inspected_ = nullptr;

    std::string toast_;
    double toastTimer_ = 0;
    bool escaped_ = false;
    double escapeDelay_ = 0;
};
