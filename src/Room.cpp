#include "Room.h"

#include <algorithm>

#include "Config.h"
#include "TextRenderer.h"
#include "UI.h"

GameObject* Room::find(const std::string& id) const {
    for (const auto& o : objects_)
        if (o->id() == id) return o.get();
    return nullptr;
}

GameObject* Room::objectAt(int x, int y) const {
    for (auto it = objects_.rbegin(); it != objects_.rend(); ++it)
        if ((*it)->hit(x, y)) return it->get();
    return nullptr;
}

bool Room::hasExit() const {
    return std::any_of(objects_.begin(), objects_.end(), [](const auto& o) { return o->isExit(); });
}

void Room::complete(GameObject& obj, Inventory& inventory) {
    if (obj.isCompleted()) return;
    obj.markCompleted();

    if (obj.hasReward()) {
        inventory.add(obj.reward());
        for (auto* o : observers_) o->onItemFound(obj.reward());
    }
    for (const auto& id : obj.unlocks()) {
        if (GameObject* target = find(id)) {
            target->unlock();
            for (auto* o : observers_) o->onObjectUnlocked(target->id(), target->name());
        }
    }
    if (obj.isExit())
        for (auto* o : observers_) o->onRoomEscaped();
}

std::vector<std::string> Room::completedIds() const {
    std::vector<std::string> out;
    for (const auto& o : objects_)
        if (o->isCompleted()) out.push_back(o->id());
    return out;
}

std::vector<std::string> Room::unlockedIds() const {
    std::vector<std::string> out;
    for (const auto& o : objects_)
        if (o->wasUnlocked()) out.push_back(o->id());
    return out;
}

void Room::restore(const std::vector<std::string>& completed, const std::vector<std::string>& unlocked) {
    for (const auto& id : unlocked)
        if (GameObject* o = find(id)) o->unlock();
    for (const auto& id : completed)
        if (GameObject* o = find(id)) {
            o->markCompleted();
            if (o->puzzle()) o->puzzle()->forceSolved();
        }
}

void Room::draw(Context& ctx) const {
    SDL_Rect full{0, 0, cfg::kScreenW, cfg::kScreenH};
    ui::drawImageOrPlaceholder(ctx, background_, full, name_, SDL_Color{40, 44, 62, 255});
    GameObject* hovered = objectAt(ctx.mouseX, ctx.mouseY);
    for (const auto& o : objects_) o->draw(ctx, o.get() == hovered);
}
