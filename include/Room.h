#pragma once
// Room.h - one level: background + objects. It is the SUBJECT in the Observer pattern.
#include <memory>
#include <string>
#include <vector>

#include "GameObject.h"
#include "Inventory.h"
#include "Observer.h"

class Room {
public:
    Room(std::string name, double timeLimit, std::string background)
        : name_(std::move(name)), timeLimit_(timeLimit), background_(std::move(background)) {}

    void addObject(std::unique_ptr<GameObject> obj) { objects_.push_back(std::move(obj)); }
    GameObject* find(const std::string& id) const;
    GameObject* objectAt(int x, int y) const;  // top-most visible object under the point
    bool hasExit() const;

    void attach(RoomObserver* o) { observers_.push_back(o); }

    // Called when a puzzle is solved (or a puzzle-less object is inspected the first time).
    // Gives the reward, unlocks other objects, and notifies observers.
    void complete(GameObject& obj, Inventory& inventory);

    // Save/load support
    std::vector<std::string> completedIds() const;
    std::vector<std::string> unlockedIds() const;
    void restore(const std::vector<std::string>& completed, const std::vector<std::string>& unlocked);

    void draw(Context& ctx) const;

    const std::string& name() const { return name_; }
    double timeLimit() const { return timeLimit_; }
    const std::vector<std::unique_ptr<GameObject>>& objects() const { return objects_; }

private:
    std::string name_;
    double timeLimit_;
    std::string background_;
    std::vector<std::unique_ptr<GameObject>> objects_;
    std::vector<RoomObserver*> observers_;
};
