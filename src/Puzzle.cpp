#include "Puzzle.h"

#include "Config.h"
#include "TextRenderer.h"
#include "Utils.h"

bool Puzzle::matches(const std::string& attempt) const {
    return util::toLower(util::trim(attempt)) == util::toLower(util::trim(answer_));
}

void Puzzle::drawFeedback(Context& ctx, int x, int y) const {
    if (feedback_.empty()) return;
    ctx.text->draw(feedback_, x, y, error_ ? col::Red : col::Green, 22);
}
