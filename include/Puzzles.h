#pragma once
// Puzzles.h - the concrete puzzle types (inheritance). All share the Puzzle interface.
#include "Puzzle.h"

// Type the answer on the keyboard (riddles AND ciphers). level file: puzzle=riddle (or cipher)
class RiddlePuzzle : public Puzzle {
public:
    using Puzzle::Puzzle;
    std::string typeName() const override { return "riddle"; }
    void handleEvent(const SDL_Event& e, Context& ctx, const SDL_Rect& area, const Item* sel) override;
    void draw(Context& ctx, const SDL_Rect& area) override;

private:
    void submit();
    std::string input_;
};

// Numeric keypad. The code length = length of the answer. level file: puzzle=code
class CodeLockPuzzle : public Puzzle {
public:
    using Puzzle::Puzzle;
    std::string typeName() const override { return "code"; }
    void handleEvent(const SDL_Event& e, Context& ctx, const SDL_Rect& area, const Item* sel) override;
    void draw(Context& ctx, const SDL_Rect& area) override;

private:
    static SDL_Rect keyRect(const SDL_Rect& area, int index);
    void pressKey(int index);
    void submit();
    std::string digits_;
};

// Click coloured buttons in the right order. answer is letters R,G,B,Y e.g. "RYGB".
// level file: puzzle=sequence
class SequencePuzzle : public Puzzle {
public:
    using Puzzle::Puzzle;
    std::string typeName() const override { return "sequence"; }
    void handleEvent(const SDL_Event& e, Context& ctx, const SDL_Rect& area, const Item* sel) override;
    void draw(Context& ctx, const SDL_Rect& area) override;

private:
    static SDL_Rect buttonRect(const SDL_Rect& area, int index);
    static SDL_Rect resetRect(const SDL_Rect& area);
    std::string seq_;
};

// Use an inventory item on the object. answer = item id. level file: puzzle=itemuse
class ItemUsePuzzle : public Puzzle {
public:
    using Puzzle::Puzzle;
    std::string typeName() const override { return "itemuse"; }
    void handleEvent(const SDL_Event& e, Context& ctx, const SDL_Rect& area, const Item* sel) override;
    void draw(Context& ctx, const SDL_Rect& area) override;
    std::string consumedItemId() const override { return solved_ ? answer_ : std::string(); }

private:
    static SDL_Rect useRect(const SDL_Rect& area);
};
