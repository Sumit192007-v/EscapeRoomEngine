#pragma once
// Inventory.h - the player's items + the bottom inventory bar (draw + click selection).
#include <vector>

#include "Context.h"
#include "Item.h"

class Inventory {
public:
    void add(const Item& item);
    bool has(const std::string& id) const;
    bool remove(const std::string& id);
    void clear();

    const std::vector<Item>& items() const { return items_; }
    const Item* selectedItem() const;  // nullptr when nothing is selected

    // Returns true if the click landed on the inventory bar (and so should not reach the room).
    bool handleClick(int x, int y);
    void draw(Context& ctx) const;

private:
    static SDL_Rect slotRect(int index);
    std::vector<Item> items_;
    int selected_ = -1;
};
