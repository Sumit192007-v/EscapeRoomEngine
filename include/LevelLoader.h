#pragma once
// LevelLoader.h - reads level text files and BUILDS Room objects (Factory pattern).
// Levels are DATA, not code: add a file in /levels and one line in levels/level_list.txt.
#include <memory>
#include <string>
#include <vector>

#include "Room.h"

class LevelLoader {
public:
    static std::vector<std::string> loadLevelList(const std::string& listPath);
    static std::unique_ptr<Room> load(const std::string& levelPath);  // throws LevelParseException
};
