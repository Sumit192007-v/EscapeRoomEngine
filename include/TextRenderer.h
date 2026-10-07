#pragma once
// TextRenderer.h - draws text with SDL_ttf (fonts cached per size).
#include <map>
#include <string>

#include "SDLWrappers.h"

class TextRenderer {
public:
    TextRenderer(SDL_Renderer* renderer, const std::string& fontPath);

    void draw(const std::string& text, int x, int y, SDL_Color c, int size = 22);
    // Wraps lines at wrapWidth pixels. Returns the height used.
    int drawWrapped(const std::string& text, int x, int y, int wrapWidth, SDL_Color c, int size = 22);
    // Centres text inside box (single line).
    void drawCentered(const std::string& text, const SDL_Rect& box, SDL_Color c, int size = 22);
    int measureWidth(const std::string& text, int size);

private:
    TTF_Font* font(int size);
    void blit(SDL_Surface* surf, int x, int y);

    SDL_Renderer* renderer_;
    std::string fontPath_;
    std::map<int, FontPtr> fonts_;
};
