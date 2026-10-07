#pragma once
// SaveManager.h - file handling: save/load the player's progress as a small text file.
#include <string>
#include <vector>

#include "Item.h"

struct SaveData {
    int levelIndex = 0;
    double timeLeft = -1;  // < 0 means "start the level with its full time"
    int score = 0;
    std::vector<std::string> completed;  // object ids already solved
    std::vector<std::string> unlocked;   // object ids that were unlocked/revealed
    std::vector<Item> items;             // inventory
};

class SaveManager {
public:
    static void save(const SaveData& data, const std::string& path);  // throws SaveFileException
    static SaveData load(const std::string& path);                    // throws SaveFileException
    static bool exists(const std::string& path);
    static void remove(const std::string& path);
};
