#pragma once
// Item.h - something the player can pick up and carry.
#include <string>

struct Item {
    std::string id;         // e.g. "brass_key" (used by puzzles to match)
    std::string name;       // e.g. "Brass Key" (shown to the player)
    std::string imagePath;  // e.g. "assets/images/items/brass_key.png"
};
