#pragma once
// DialLockPuzzle - NEW puzzle type (my contribution).
// A combination lock with one rotating dial per digit: click the up/down arrows
// above/below each digit, then press "Open". Level-file usage:  puzzle=dial
#include "Puzzle.h"

class DialLockPuzzle : public Puzzle {
public:
    DialLockPuzzle(const std::string& question, const std::string& answer);

    std::string typeName() const override { return "dial"; }
    void handleEvent(const SDL_Event& e, Context& ctx, const SDL_Rect& area, const Item* selected) override;
    void draw(Context& ctx, const SDL_Rect& area) override;

private:
    std::string digits_;  // current value shown on the dials, e.g. "042"

    static SDL_Rect upRect(const SDL_Rect& area, int i);
    static SDL_Rect downRect(const SDL_Rect& area, int i);
    static SDL_Rect digitRect(const SDL_Rect& area, int i);
    static SDL_Rect openRect(const SDL_Rect& area);
    void turn(int dial, int delta);
};
