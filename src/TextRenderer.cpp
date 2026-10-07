#include "TextRenderer.h"

TextRenderer::TextRenderer(SDL_Renderer* renderer, const std::string& fontPath)
    : renderer_(renderer), fontPath_(fontPath) {
    font(22);  // load once now so a missing font fails immediately with a clear message
}

TTF_Font* TextRenderer::font(int size) {
    auto it = fonts_.find(size);
    if (it != fonts_.end()) return it->second.get();
    TTF_Font* f = TTF_OpenFont(fontPath_.c_str(), size);
    if (!f)
        throw AssetLoadException("cannot open font '" + fontPath_ + "': " + TTF_GetError());
    fonts_[size] = FontPtr(f);
    return f;
}

void TextRenderer::blit(SDL_Surface* surf, int x, int y) {
    if (!surf) return;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer_, surf);
    if (tex) {
        SDL_Rect dst{x, y, surf->w, surf->h};
        SDL_RenderCopy(renderer_, tex, nullptr, &dst);
        SDL_DestroyTexture(tex);
    }
    SDL_FreeSurface(surf);
}

void TextRenderer::draw(const std::string& text, int x, int y, SDL_Color c, int size) {
    if (text.empty()) return;
    blit(TTF_RenderUTF8_Blended(font(size), text.c_str(), c), x, y);
}

int TextRenderer::drawWrapped(const std::string& text, int x, int y, int wrapWidth, SDL_Color c,
                              int size) {
    if (text.empty()) return 0;
    SDL_Surface* s = TTF_RenderUTF8_Blended_Wrapped(font(size), text.c_str(), c,
                                                    static_cast<Uint32>(wrapWidth));
    int h = s ? s->h : 0;
    blit(s, x, y);
    return h;
}

int TextRenderer::measureWidth(const std::string& text, int size) {
    int w = 0, h = 0;
    TTF_SizeUTF8(font(size), text.c_str(), &w, &h);
    return w;
}

void TextRenderer::drawCentered(const std::string& text, const SDL_Rect& box, SDL_Color c, int size) {
    if (text.empty()) return;
    int w = 0, h = 0;
    TTF_SizeUTF8(font(size), text.c_str(), &w, &h);
    draw(text, box.x + (box.w - w) / 2, box.y + (box.h - h) / 2, c, size);
}
