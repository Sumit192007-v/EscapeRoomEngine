#pragma once
// SDLWrappers.h - RAII wrappers so SDL resources are always released.
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <memory>
#include <string>

#include "Exceptions.h"

struct WindowDeleter   { void operator()(SDL_Window* p)   const { if (p) SDL_DestroyWindow(p); } };
struct RendererDeleter { void operator()(SDL_Renderer* p) const { if (p) SDL_DestroyRenderer(p); } };
struct TextureDeleter  { void operator()(SDL_Texture* p)  const { if (p) SDL_DestroyTexture(p); } };
struct FontDeleter     { void operator()(TTF_Font* p)     const { if (p) TTF_CloseFont(p); } };

using WindowPtr   = std::unique_ptr<SDL_Window, WindowDeleter>;
using RendererPtr = std::unique_ptr<SDL_Renderer, RendererDeleter>;
using TexturePtr  = std::unique_ptr<SDL_Texture, TextureDeleter>;
using FontPtr     = std::unique_ptr<TTF_Font, FontDeleter>;

// Initialises SDL, SDL_image, SDL_ttf in the constructor and shuts them down in the destructor.
class SdlSystem {
public:
    SdlSystem() {
        if (SDL_Init(SDL_INIT_VIDEO) != 0)
            throw GameException(std::string("SDL_Init failed: ") + SDL_GetError());
        if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) == 0) {
            std::string err = IMG_GetError();
            SDL_Quit();
            throw GameException("IMG_Init failed: " + err);
        }
        if (TTF_Init() != 0) {
            std::string err = TTF_GetError();
            IMG_Quit();
            SDL_Quit();
            throw GameException("TTF_Init failed: " + err);
        }
    }
    ~SdlSystem() {
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }
    SdlSystem(const SdlSystem&) = delete;
    SdlSystem& operator=(const SdlSystem&) = delete;
};
