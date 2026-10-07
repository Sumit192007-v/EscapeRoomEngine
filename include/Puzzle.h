#pragma once
// Puzzle.h - ABSTRACT base class for every puzzle type (abstraction + polymorphism).
// To add a new puzzle type: derive from Puzzle, then register it in PuzzleFactory.
#include <string>
#include <utility>

#include "Context.h"
#include "Item.h"

class Puzzle {
public:
    Puzzle(std::string question, std::string answer)
        : question_(std::move(question)), answer_(std::move(answer)) {}
    virtual ~Puzzle() = default;

    virtual std::string typeName() const = 0;
    // 'area' is the part of the inspect panel reserved for the puzzle.
    virtual void handleEvent(const SDL_Event& e, Context& ctx, const SDL_Rect& area,
                             const Item* selectedItem) = 0;
    virtual void draw(Context& ctx, const SDL_Rect& area) = 0;
    // If solving consumes an inventory item, return its id (else empty).
    virtual std::string consumedItemId() const { return {}; }

    bool isSolved() const { return solved_; }
    void forceSolved() { solved_ = true; }  // used when loading a save
    const std::string& question() const { return question_; }

protected:
    bool matches(const std::string& attempt) const;  // case-insensitive, ignores spaces at the ends
    void succeed(const std::string& msg) { solved_ = true; feedback_ = msg; error_ = false; }
    void fail(const std::string& msg) { feedback_ = msg; error_ = true; }
    void drawFeedback(Context& ctx, int x, int y) const;

    std::string question_;
    std::string answer_;
    std::string feedback_;
    bool solved_ = false;
    bool error_ = false;
};
