#pragma once
// Context.h - small bundle of shared services handed to anything that draws.
#include <SDL.h>

class TextureManager;
class TextRenderer;

struct Context {
    SDL_Renderer*   renderer = nullptr;
    TextureManager* textures = nullptr;
    TextRenderer*   text     = nullptr;
    int mouseX = 0;  // logical coordinates (1280x720)
    int mouseY = 0;
};
