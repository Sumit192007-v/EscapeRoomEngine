#include "Inventory.h"

#include <algorithm>

#include "Config.h"
#include "TextRenderer.h"
#include "UI.h"

SDL_Rect Inventory::slotRect(int index) { return SDL_Rect{20 + index * 84, 635, 70, 70}; }

void Inventory::add(const Item& item) {
    if (!has(item.id)) items_.push_back(item);
}

bool Inventory::has(const std::string& id) const {
    return std::any_of(items_.begin(), items_.end(), [&](const Item& i) { return i.id == id; });
}

bool Inventory::remove(const std::string& id) {
    auto it = std::find_if(items_.begin(), items_.end(), [&](const Item& i) { return i.id == id; });
    if (it == items_.end()) return false;
    items_.erase(it);
    selected_ = -1;
    return true;
}

void Inventory::clear() {
    items_.clear();
    selected_ = -1;
}

const Item* Inventory::selectedItem() const {
    if (selected_ < 0 || selected_ >= static_cast<int>(items_.size())) return nullptr;
    return &items_[selected_];
}

bool Inventory::handleClick(int x, int y) {
    for (int i = 0; i < static_cast<int>(items_.size()); ++i) {
        if (ui::contains(slotRect(i), x, y)) {
            selected_ = (selected_ == i) ? -1 : i;  // click again to deselect
            return true;
        }
    }
    return ui::contains(cfg::kInventoryBar, x, y);
}

void Inventory::draw(Context& ctx) const {
    ui::fillRect(ctx.renderer, cfg::kInventoryBar, SDL_Color{16, 16, 24, 235});
    ui::outlineRect(ctx.renderer, cfg::kInventoryBar, col::Grey, 1);

    if (items_.empty()) {
        ctx.text->draw("Inventory is empty - inspect objects to find items.", 20, 660, col::Grey, 20);
        return;
    }
    for (int i = 0; i < static_cast<int>(items_.size()); ++i) {
        SDL_Rect r = slotRect(i);
        ui::fillRect(ctx.renderer, r, SDL_Color{40, 42, 60, 255});
        std::string shortName = items_[i].name.substr(0, 9);
        ui::drawImageOrPlaceholder(ctx, items_[i].imagePath, r, shortName, SDL_Color{90, 90, 120, 255});
        ui::outlineRect(ctx.renderer, r, i == selected_ ? col::Gold : col::Grey, i == selected_ ? 3 : 1);
    }
    if (const Item* s = selectedItem())
        ctx.text->draw("Selected: " + s->name, 900, 660, col::Gold, 22);
}
