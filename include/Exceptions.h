#pragma once
// Exceptions.h - custom exception hierarchy (exception handling requirement).
#include <stdexcept>
#include <string>

class GameException : public std::runtime_error {
public:
    explicit GameException(const std::string& msg) : std::runtime_error(msg) {}
};

class AssetLoadException : public GameException {
public:
    explicit AssetLoadException(const std::string& msg) : GameException("Asset error: " + msg) {}
};

class LevelParseException : public GameException {
public:
    explicit LevelParseException(const std::string& msg) : GameException("Level file error: " + msg) {}
};

class SaveFileException : public GameException {
public:
    explicit SaveFileException(const std::string& msg) : GameException("Save file error: " + msg) {}
};
