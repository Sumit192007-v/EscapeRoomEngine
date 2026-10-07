# Escape Room Engine (C++17 + SDL2 + SDL2_image + SDL2_ttf)

A 2D point-and-click escape room ENGINE with a 3-level demo game.
"The game is just a demo. The engine is the project."

## 1. Where to keep the project (do this first)
Copy the whole `EscapeRoomEngine` folder to a SHORT, simple path:

    C:\Dev\EscapeRoomEngine        <-- recommended

Avoid: OneDrive / Desktop / Documents (sync problems), paths with spaces or very long paths,
and never run the game from inside the zip file (extract it first).

## 2. Download SDL (3 downloads, "VC" development packages, 64-bit)
From https://github.com/libsdl-org (Releases of each repo) download the `*-devel-*-VC.zip` files:
- SDL2        -> SDL2-devel-2.x.x-VC.zip
- SDL2_image  -> SDL2_image-devel-2.8.x-VC.zip   (any 2.8.x is fine)
- SDL2_ttf    -> SDL2_ttf-devel-2.2x.x-VC.zip

Open each zip, go INTO the folder inside it (e.g. `SDL2-2.30.x`) and copy its CONTENTS so you get:

    EscapeRoomEngine/third_party/SDL2/include/SDL.h
    EscapeRoomEngine/third_party/SDL2/lib/x64/SDL2.lib   (+ SDL2main.lib, SDL2.dll)
    EscapeRoomEngine/third_party/SDL2_image/include/SDL_image.h
    EscapeRoomEngine/third_party/SDL2_image/lib/x64/SDL2_image.lib  (+ SDL2_image.dll)
    EscapeRoomEngine/third_party/SDL2_ttf/include/SDL_ttf.h
    EscapeRoomEngine/third_party/SDL2_ttf/lib/x64/SDL2_ttf.lib      (+ SDL2_ttf.dll)

(Prefer another location? Edit the 3 lines SDL2_DIR / SDL2_IMAGE_DIR / SDL2_TTF_DIR in
EscapeRoomEngine.vcxproj.)

## 3. Build and run (Visual Studio 2022 Community)
1. Install the workload "Desktop development with C++".
2. Double-click `EscapeRoomEngine.sln`.
3. Top toolbar: choose **x64** (NOT x86) and Debug or Release.
4. Press **F5** (or Ctrl+F5). The DLLs are copied next to the exe automatically.
The game finds `assets/` and `levels/` by itself even if you start the exe from `bin/`.

## 4. Controls
- Mouse: hover an object (gold outline), click to INSPECT it.
- Puzzle windows: type answers (riddle / code) or click buttons; Enter = submit.
- Inventory (bottom bar): click an item to select it, then press "Use selected item" on a lock.
- ESC: close window / back to menu (auto-saves).  F5: save.  Menu -> Continue to resume.

## 5. Folder structure
    EscapeRoomEngine/
      EscapeRoomEngine.sln / .vcxproj      Visual Studio project
      include/                             headers (.h)
      src/                                 sources (.cpp), main.cpp
      levels/level_list.txt, level1..3.txt DATA-DRIVEN levels (no C++ needed to add levels)
      assets/images/menu_bg.png            menu background (1280x720)
      assets/images/levelN/*.png           room background (bg.png 1280x720) + object images
      assets/images/items/*.png            inventory icons (70x70)
      assets/fonts/font.ttf                DejaVu Sans (free licence) - replace with any .ttf, keep the name
      saves/                               savegame.txt appears here
      tools/make_placeholders.py           creates placeholder PNGs for any missing image
      third_party/                         put SDL2 / SDL2_image / SDL2_ttf here (step 2)

## 6. Architecture (for your OOP report)
| Concept | Where |
|---|---|
| Abstraction | `Puzzle` (abstract), `GameState` (abstract), `RoomObserver` (interface) |
| Inheritance + polymorphism | `RiddlePuzzle`, `CodeLockPuzzle`, `SequencePuzzle`, `ItemUsePuzzle` : `Puzzle`; `MenuState`, `PlayState`, `ResultState` : `GameState` |
| Encapsulation | private members + getters in `GameObject`, `Room`, `Inventory` |
| Composition | `Room` has-a `GameObject`s; `GameObject` has-a `Puzzle` + reward `Item`; `Game` has-a services |
| Factory pattern | `PuzzleFactory`, `LevelLoader` (text file -> Room) |
| Observer pattern | `Room` notifies `PlayState` (unlock / item found / escaped) |
| State pattern | `Game` switches `GameState`s |
| RAII / smart pointers | `SDLWrappers.h` (unique_ptr + deleters), `SdlSystem` |
| Exception handling | `Exceptions.h`; caught in `Game::startLevel`, `main` |
| File handling | `LevelLoader` (levels), `SaveManager` (save/load) |
| Resource cache | `TextureManager` (each PNG loaded once; missing PNG -> placeholder box) |

## 7. Add a new level (no C++ changes)
1. Copy `levels/level3.txt` to `levels/level4.txt`, edit the objects.
2. Add `levels/level4.txt` as a new line in `levels/level_list.txt`.
3. Put your PNGs where the file says (or run `python tools/make_placeholders.py` for placeholders).

Object keys: id, name, x, y, w, h, image, inspect_image, description, hidden, locked,
locked_message, exit, puzzle (riddle|cipher|code|sequence|itemuse), question, answer,
unlocks (comma list of ids), reward_id, reward_name, reward_image.
Keep objects above y=620 (the inventory bar covers the bottom 100 px).

## 8. Add a new puzzle type
1. Derive a class from `Puzzle` in `Puzzles.h/.cpp` (implement `typeName`, `handleEvent`, `draw`).
2. Add one `if` line in `PuzzleFactory::create`.
3. Use `puzzle=yourtype` in a level file.

## 9. Common errors
- LNK1104 cannot open SDL2.lib -> SDL files are not in `third_party/.../lib/x64`, or platform is x86. Choose x64.
- Cannot open include file SDL.h -> step 2 copied the wrong folder level (SDL.h must be in `third_party/SDL2/include/`).
- "SDL2.dll was not found" -> run from Visual Studio, or copy the 3 DLLs next to the exe.
- "Asset error: cannot open font" -> `assets/fonts/font.ttf` is missing.
- Pink/colour boxes instead of pictures -> PNG missing (it is a placeholder). Console prints which file.
- Typing does nothing in a riddle -> click the game window first so it has keyboard focus.
