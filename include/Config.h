#pragma once
// Config.h - all fixed numbers/paths/colours in ONE place.
#include <SDL.h>

namespace cfg {
constexpr int kScreenW = 1280;
constexpr int kScreenH = 720;
constexpr const char* kTitle = "Escape Room Engine";

// Paths are relative to the project root folder (see README).
constexpr const char* kFontPath  = "assets/fonts/font.ttf";
constexpr const char* kLevelList = "levels/level_list.txt";
constexpr const char* kSavePath  = "saves/savegame.txt";
constexpr const char* kMenuBg    = "assets/images/menu_bg.png";

// UI layout (logical 1280x720 coordinates)
inline constexpr SDL_Rect kInventoryBar{0, 620, 1280, 100};
inline constexpr SDL_Rect kInspectPanel{240, 40, 800, 560};
}  // namespace cfg

namespace col {
inline constexpr SDL_Color White{255, 255, 255, 255};
inline constexpr SDL_Color Black{0, 0, 0, 255};
inline constexpr SDL_Color Gold{255, 210, 70, 255};
inline constexpr SDL_Color Green{90, 220, 120, 255};
inline constexpr SDL_Color Red{240, 90, 90, 255};
inline constexpr SDL_Color Grey{170, 170, 180, 255};
inline constexpr SDL_Color Panel{28, 30, 44, 245};
inline constexpr SDL_Color Dark{14, 15, 24, 255};
}  // namespace col
