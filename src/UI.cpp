#include "UI.h"

#include "Config.h"
#include "TextRenderer.h"
#include "TextureManager.h"

namespace ui {
bool contains(const SDL_Rect& r, int x, int y) {
    return x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h;
}

void fillRect(SDL_Renderer* r, const SDL_Rect& rect, SDL_Color c) {
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    SDL_RenderFillRect(r, &rect);
}

void outlineRect(SDL_Renderer* r, const SDL_Rect& rect, SDL_Color c, int thickness) {
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    for (int i = 0; i < thickness; ++i) {
        SDL_Rect o{rect.x + i, rect.y + i, rect.w - 2 * i, rect.h - 2 * i};
        SDL_RenderDrawRect(r, &o);
    }
}

void drawButton(Context& ctx, const SDL_Rect& rect, const std::string& label, bool enabled,
                int fontSize) {
    bool hover = enabled && contains(rect, ctx.mouseX, ctx.mouseY);
    SDL_Color bg = !enabled ? SDL_Color{50, 50, 60, 255}
                            : (hover ? SDL_Color{90, 110, 190, 255} : SDL_Color{60, 75, 140, 255});
    fillRect(ctx.renderer, rect, bg);
    outlineRect(ctx.renderer, rect, hover ? col::Gold : col::Grey, 2);
    ctx.text->drawCentered(label, rect, enabled ? col::White : SDL_Color{120, 120, 130, 255}, fontSize);
}

void drawImageOrPlaceholder(Context& ctx, const std::string& imagePath, const SDL_Rect& dst,
                            const std::string& label, SDL_Color fallback) {
    SDL_Texture* tex = ctx.textures->get(imagePath);
    if (tex) {
        SDL_RenderCopy(ctx.renderer, tex, nullptr, &dst);
        return;
    }
    fillRect(ctx.renderer, dst, fallback);
    outlineRect(ctx.renderer, dst, col::White, 1);
    ctx.text->drawCentered(label, dst, col::White, 16);
}
}  // namespace ui
