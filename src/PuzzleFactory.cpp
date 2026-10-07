#include "PuzzleFactory.h"

#include "Exceptions.h"
#include "DialLockPuzzle.h"
#include "Puzzles.h"
#include "Utils.h"

std::unique_ptr<Puzzle> PuzzleFactory::create(const std::string& type, const std::string& question,
                                              const std::string& answer) {
    const std::string t = util::toLower(type);
    if (t == "riddle" || t == "cipher") return std::make_unique<RiddlePuzzle>(question, answer);
    if (t == "code")     return std::make_unique<CodeLockPuzzle>(question, answer);
    if (t == "sequence") return std::make_unique<SequencePuzzle>(question, answer);
    if (t == "itemuse")  return std::make_unique<ItemUsePuzzle>(question, answer);
    if (t == "dial")     return std::make_unique<DialLockPuzzle>(question, answer);
    throw LevelParseException("unknown puzzle type '" + type + "'");
}
