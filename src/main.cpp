// main.cpp - entry point. All errors end up here (exception handling).
#include <filesystem>
#include <iostream>

#include "Game.h"

namespace fs = std::filesystem;

// Makes the game work no matter where the .exe is started from: finds the project root
// (the folder that contains "levels" and "assets") and makes it the working directory.
static void enterProjectRoot() {
    auto isRoot = [](const fs::path& p) { return fs::exists(p / "levels") && fs::exists(p / "assets"); };
    auto climb = [&](fs::path p) {
        for (int i = 0; i < 6; ++i) {
            if (isRoot(p)) { fs::current_path(p); return true; }
            if (!p.has_parent_path() || p == p.parent_path()) break;
            p = p.parent_path();
        }
        return false;
    };
    if (climb(fs::current_path())) return;
    if (char* base = SDL_GetBasePath()) {
        fs::path b(base);
        SDL_free(base);
        climb(b);
    }
}

int main(int /*argc*/, char* /*argv*/[]) {
    try {
        enterProjectRoot();
        Game game;
        game.run();
    } catch (const GameException& e) {
        std::cerr << e.what() << "\n";
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Escape Room Engine - Error", e.what(), nullptr);
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << "\n";
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Escape Room Engine - Unexpected error", e.what(), nullptr);
        return 2;
    }
    return 0;
}
