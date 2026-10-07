#include "SaveManager.h"

#include <filesystem>
#include <fstream>

#include "Exceptions.h"
#include "Utils.h"

namespace fs = std::filesystem;

void SaveManager::save(const SaveData& d, const std::string& path) {
    try {
        fs::path p(path);
        if (p.has_parent_path()) fs::create_directories(p.parent_path());
    } catch (const fs::filesystem_error& e) {
        throw SaveFileException(e.what());
    }
    std::ofstream out(path);
    if (!out) throw SaveFileException("cannot write '" + path + "'");
    out << "level=" << d.levelIndex << "\n";
    out << "time=" << d.timeLeft << "\n";
    out << "score=" << d.score << "\n";
    for (const auto& id : d.completed) out << "completed=" << id << "\n";
    for (const auto& id : d.unlocked) out << "unlocked=" << id << "\n";
    for (const auto& it : d.items) out << "item=" << it.id << "|" << it.name << "|" << it.imagePath << "\n";
    if (!out) throw SaveFileException("write failed for '" + path + "'");
}

SaveData SaveManager::load(const std::string& path) {
    std::ifstream in(path);
    if (!in) throw SaveFileException("no save file found at '" + path + "'");
    SaveData d;
    std::string line;
    try {
        while (std::getline(in, line)) {
            line = util::trim(line);
            size_t eq = line.find('=');
            if (line.empty() || eq == std::string::npos) continue;
            std::string key = line.substr(0, eq), val = line.substr(eq + 1);
            if (key == "level") d.levelIndex = std::stoi(val);
            else if (key == "time") d.timeLeft = std::stod(val);
            else if (key == "score") d.score = std::stoi(val);
            else if (key == "completed") d.completed.push_back(val);
            else if (key == "unlocked") d.unlocked.push_back(val);
            else if (key == "item") {
                auto parts = util::split(val, '|');
                if (parts.size() >= 2) d.items.push_back(Item{parts[0], parts[1], parts.size() > 2 ? parts[2] : ""});
            }
        }
    } catch (const std::exception&) {
        throw SaveFileException("save file is corrupted");
    }
    return d;
}

bool SaveManager::exists(const std::string& path) { return fs::exists(path); }

void SaveManager::remove(const std::string& path) {
    std::error_code ec;
    fs::remove(path, ec);
}
