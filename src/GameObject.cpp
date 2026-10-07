#include "GameObject.h"

#include <algorithm>

#include "Config.h"
#include "TextRenderer.h"
#include "UI.h"

bool GameObject::hit(int x, int y) const { return !hidden_ && ui::contains(bounds_, x, y); }

void GameObject::draw(Context& ctx, bool hovered) const {
    if (hidden_) return;
    if (!imagePath_.empty())
        ui::drawImageOrPlaceholder(ctx, imagePath_, bounds_, name_, SDL_Color{ 70, 90, 130, 200 });
    if (showHotspots) {
        ui::outlineRect(ctx.renderer, bounds_, SDL_Color{ 0, 255, 255, 255 }, 2);
        ctx.text->draw(id_, bounds_.x + 4, bounds_.y + 2, SDL_Color{ 0, 255, 255, 255 }, 16);
    }
    if (completed_) {  // small green dot = done
        SDL_Rect dot{bounds_.x + bounds_.w - 18, bounds_.y + 6, 12, 12};
        ui::fillRect(ctx.renderer, dot, col::Green);
    } else if (locked_) {  // small red dot = locked
        SDL_Rect dot{bounds_.x + bounds_.w - 18, bounds_.y + 6, 12, 12};
        ui::fillRect(ctx.renderer, dot, col::Red);
    }
    if (hovered) {
        ui::outlineRect(ctx.renderer, bounds_, col::Gold, 3);
        int w = ctx.text->measureWidth(name_, 20) + 16;
        SDL_Rect tag{bounds_.x, std::max(0, bounds_.y - 30), w, 28};
        ui::fillRect(ctx.renderer, tag, SDL_Color{0, 0, 0, 200});
        ctx.text->drawCentered(name_, tag, col::Gold, 20);
    }
}
