#pragma once
// PuzzleFactory.h - Factory pattern: builds the right Puzzle subclass from a text type name.
#include <memory>
#include <string>

#include "Puzzle.h"

class PuzzleFactory {
public:
    // Throws LevelParseException for an unknown type.
    static std::unique_ptr<Puzzle> create(const std::string& type, const std::string& question,
                                          const std::string& answer);
};
