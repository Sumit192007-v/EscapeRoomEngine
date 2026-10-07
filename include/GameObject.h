#pragma once
// GameObject.h - one clickable thing in a room (composition: may HAVE a Puzzle and a reward Item).
#include <memory>
#include <string>
#include <vector>

#include "Context.h"
#include "Item.h"
#include "Puzzle.h"

class GameObject {
public:
    GameObject(std::string id, std::string name, SDL_Rect bounds)
        : id_(std::move(id)), name_(std::move(name)), bounds_(bounds) {}

    // ---- configuration (called by the level loader) ----
    void setImage(std::string p)        { imagePath_ = std::move(p); }
    void setInspectImage(std::string p) { inspectImagePath_ = std::move(p); }
    void setDescription(std::string d)  { description_ = std::move(d); }
    void setLockedMessage(std::string m){ lockedMessage_ = std::move(m); }
    void setHidden(bool h)              { hidden_ = h; }
    void setLocked(bool l)              { locked_ = l; }
    void setExit(bool e)                { exit_ = e; }
    void setReward(const Item& item)    { reward_ = item; hasReward_ = true; }
    void setUnlocks(std::vector<std::string> ids) { unlocks_ = std::move(ids); }
    void setPuzzle(std::unique_ptr<Puzzle> p)     { puzzle_ = std::move(p); }

    // ---- state ----
    void unlock()        { locked_ = false; hidden_ = false; wasUnlocked_ = true; }
    void markCompleted() { completed_ = true; }

    // ---- getters ----
    const std::string& id() const              { return id_; }
    const std::string& name() const            { return name_; }
    const std::string& description() const     { return description_; }
    const std::string& lockedMessage() const   { return lockedMessage_; }
    const std::string& imagePath() const       { return imagePath_; }
    const std::string& inspectImagePath() const{ return inspectImagePath_; }
    const SDL_Rect& bounds() const             { return bounds_; }
    bool isHidden() const    { return hidden_; }
    bool isLocked() const    { return locked_; }
    bool isExit() const      { return exit_; }
    bool isCompleted() const { return completed_; }
    bool wasUnlocked() const { return wasUnlocked_; }
    bool hasReward() const   { return hasReward_; }
    const Item& reward() const { return reward_; }
    const std::vector<std::string>& unlocks() const { return unlocks_; }
    Puzzle* puzzle() const   { return puzzle_.get(); }

    bool hit(int x, int y) const;
    static inline bool showHotspots = false;  // F1 in game: outline every clickable area
    void draw(Context& ctx, bool hovered) const;

private:
    std::string id_, name_, description_, imagePath_, inspectImagePath_;
    std::string lockedMessage_ = "It won't budge. Maybe something else in the room is the key.";
    SDL_Rect bounds_;
    bool hidden_ = false, locked_ = false, exit_ = false, completed_ = false, wasUnlocked_ = false;
    bool hasReward_ = false;
    Item reward_;
    std::vector<std::string> unlocks_;
    std::unique_ptr<Puzzle> puzzle_;
};
