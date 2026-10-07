#include "LevelLoader.h"

#include <fstream>
#include <map>

#include "Exceptions.h"
#include "PuzzleFactory.h"
#include "Utils.h"

namespace {
struct Section {
    std::string name;
    std::map<std::string, std::string> kv;
    int line = 0;
};

std::string get(const Section& s, const std::string& key, const std::string& def = "") {
    auto it = s.kv.find(key);
    return it == s.kv.end() ? def : it->second;
}

bool getBool(const Section& s, const std::string& key) {
    std::string v = util::toLower(get(s, key));
    return v == "true" || v == "1" || v == "yes";
}

int getInt(const Section& s, const std::string& key) {
    auto it = s.kv.find(key);
    if (it == s.kv.end())
        throw LevelParseException("[" + s.name + "] at line " + std::to_string(s.line) + " is missing '" + key + "'");
    try {
        return std::stoi(it->second);
    } catch (const std::exception&) {
        throw LevelParseException("'" + key + "' must be a number near line " + std::to_string(s.line));
    }
}
}  // namespace

std::vector<std::string> LevelLoader::loadLevelList(const std::string& listPath) {
    std::ifstream in(listPath);
    if (!in) throw LevelParseException("cannot open level list '" + listPath + "'");
    std::vector<std::string> files;
    std::string line;
    while (std::getline(in, line)) {
        line = util::trim(line);
        if (!line.empty() && line[0] != '#') files.push_back(line);
    }
    if (files.empty()) throw LevelParseException("level list '" + listPath + "' is empty");
    return files;
}

std::unique_ptr<Room> LevelLoader::load(const std::string& path) {
    std::ifstream in(path);
    if (!in) throw LevelParseException("cannot open '" + path + "'");

    // 1) read the file into sections
    std::vector<Section> sections;
    std::string raw;
    int lineNo = 0;
    while (std::getline(in, raw)) {
        ++lineNo;
        std::string line = util::trim(raw);
        if (line.empty() || line[0] == '#') continue;
        if (line.front() == '[' && line.back() == ']') {
            sections.push_back(Section{util::toLower(line.substr(1, line.size() - 2)), {}, lineNo});
            continue;
        }
        size_t eq = line.find('=');
        if (eq == std::string::npos || sections.empty())
            throw LevelParseException(path + " line " + std::to_string(lineNo) + ": expected key=value");
        sections.back().kv[util::trim(line.substr(0, eq))] = util::trim(line.substr(eq + 1));
    }

    // 2) build the Room
    const Section* levelSec = nullptr;
    for (const auto& s : sections)
        if (s.name == "level") { levelSec = &s; break; }
    if (!levelSec) throw LevelParseException(path + ": missing [Level] section");

    double time = 300;
    try { time = std::stod(get(*levelSec, "time", "300")); }
    catch (const std::exception&) { throw LevelParseException(path + ": 'time' must be a number"); }

    auto room = std::make_unique<Room>(get(*levelSec, "name", "Unnamed Room"), time, get(*levelSec, "background"));

    // 3) build every object (and its puzzle/reward)
    std::vector<std::pair<std::string, std::vector<std::string>>> unlockRefs;
    for (const auto& s : sections) {
        if (s.name != "object") continue;
        std::string id = get(s, "id");
        if (id.empty()) throw LevelParseException(path + ": [Object] at line " + std::to_string(s.line) + " has no id");
        if (room->find(id)) throw LevelParseException(path + ": duplicate object id '" + id + "'");

        SDL_Rect r{getInt(s, "x"), getInt(s, "y"), getInt(s, "w"), getInt(s, "h")};
        auto obj = std::make_unique<GameObject>(id, get(s, "name", id), r);
        obj->setImage(get(s, "image"));
        obj->setInspectImage(get(s, "inspect_image"));
        obj->setDescription(util::unescapeNewlines(get(s, "description")));
        if (!get(s, "locked_message").empty()) obj->setLockedMessage(util::unescapeNewlines(get(s, "locked_message")));
        obj->setHidden(getBool(s, "hidden"));
        obj->setLocked(getBool(s, "locked"));
        obj->setExit(getBool(s, "exit"));

        if (!get(s, "reward_id").empty())
            obj->setReward(Item{get(s, "reward_id"), get(s, "reward_name", get(s, "reward_id")), get(s, "reward_image")});

        std::vector<std::string> unlocks = util::split(get(s, "unlocks"), ',');
        obj->setUnlocks(unlocks);
        unlockRefs.emplace_back(id, unlocks);

        if (!get(s, "puzzle").empty()) {
            if (get(s, "answer").empty())
                throw LevelParseException(path + ": object '" + id + "' has a puzzle but no 'answer'");
            obj->setPuzzle(PuzzleFactory::create(get(s, "puzzle"), util::unescapeNewlines(get(s, "question")), get(s, "answer")));
        }
        room->addObject(std::move(obj));
    }

    // 4) validate
    for (const auto& [owner, targets] : unlockRefs)
        for (const auto& t : targets)
            if (!room->find(t))
                throw LevelParseException(path + ": object '" + owner + "' unlocks unknown object '" + t + "'");
    if (!room->hasExit()) throw LevelParseException(path + ": no object has exit=true");
    return room;
}
