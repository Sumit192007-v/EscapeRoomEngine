#include "TextureManager.h"

#include <iostream>

SDL_Texture* TextureManager::get(const std::string& path) {
    if (path.empty()) return nullptr;
    auto it = cache_.find(path);
    if (it != cache_.end()) return it->second.get();
    if (missing_.count(path)) return nullptr;

    SDL_Texture* raw = IMG_LoadTexture(renderer_, path.c_str());
    if (!raw) {
        std::cerr << "[TextureManager] Missing image '" << path << "' (" << IMG_GetError()
                  << ") - using placeholder\n";
        missing_.insert(path);
        return nullptr;
    }
    SDL_SetTextureBlendMode(raw, SDL_BLENDMODE_BLEND);
    cache_[path] = TexturePtr(raw);
    return raw;
}
