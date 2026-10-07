#pragma once
// UI.h - small drawing helpers (buttons, rectangles, image-or-placeholder).
#include <string>

#include "Context.h"

namespace ui {
bool contains(const SDL_Rect& r, int x, int y);
void fillRect(SDL_Renderer* r, const SDL_Rect& rect, SDL_Color c);
void outlineRect(SDL_Renderer* r, const SDL_Rect& rect, SDL_Color c, int thickness = 2);
void drawButton(Context& ctx, const SDL_Rect& rect, const std::string& label, bool enabled = true,
                int fontSize = 22);
// Draws the PNG if it exists, otherwise a coloured box with a label (so you can test without art).
void drawImageOrPlaceholder(Context& ctx, const std::string& imagePath, const SDL_Rect& dst,
                            const std::string& label, SDL_Color fallback);
}  // namespace ui
