#pragma once
// TextureManager.h - loads each image only once (cache). Returns nullptr if a file is missing
// so the game can draw a coloured placeholder instead of crashing.
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "SDLWrappers.h"

class TextureManager {
public:
    explicit TextureManager(SDL_Renderer* renderer) : renderer_(renderer) {}
    SDL_Texture* get(const std::string& path);

private:
    SDL_Renderer* renderer_;
    std::unordered_map<std::string, TexturePtr> cache_;
    std::unordered_set<std::string> missing_;
};
