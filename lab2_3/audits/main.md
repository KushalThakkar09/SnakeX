# Code Smell Audit: Main Branch Baseline (Commit 5fc6d9d)

This audit documents the code smells identified in the single-player baseline version of the Snake game (`code/game.cpp`).

---

## Bloaters

### Primitive Obsession
* **Location**: [game.cpp:364](file:///c:/Users/Kushal/OneDrive/Documents/sem-3%20lab/ac/SnakeX/code/game.cpp#L364)
* **Confidence Level**: C3 (High)
* **Description**: The `GameBoard::place` method takes `int x` and `int y` as separate primitives instead of using the `Position` struct. This scatters raw coordinate values and bypasses the type-safe structure used elsewhere.
* **Suggested Refactoring**: Change parameter to `Position` struct.

---

## Object-Orientation Abusers

### Switch Statement (If-Else Chain on Symbol Types)
* **Location**: [game.cpp:390-394](file:///c:/Users/Kushal/OneDrive/Documents/sem-3%20lab/ac/SnakeX/code/game.cpp#L390-L394)
* **Confidence Level**: C3 (High)
* **Description**: `GameBoard::render` uses an if-else chain to inspect the string contents of grid cells (`EMOJI_FOOD`, `EMOJI_SNAKE_HEAD`, etc.) to apply ANSI colors. This is a type-code dispatch pattern that should be handled polymorphically or with a lookup table.
* **Suggested Refactoring**: Replace with a map or dictionary lookup associating entity symbols with color codes.

### Switch Statement (Direction Enum)
* **Location**: [game.cpp:323-329](file:///c:/Users/Kushal/OneDrive/Documents/sem-3%20lab/ac/SnakeX/code/game.cpp#L323-L329)
* **Confidence Level**: C2 (Medium)
* **Description**: `Snake::move` relies on a `switch(current)` block to update coordinates based on the current movement direction. While simple, it requires modification if new movement dynamics are added.
* **Suggested Refactoring**: Map `Direction` to coordinate delta structures.

---

## Dispensables

### Duplicate Code
* **Location**: [game.cpp:426-430](file:///c:/Users/Kushal/OneDrive/Documents/sem-3%20lab/ac/SnakeX/code/game.cpp#L426-L430) and [game.cpp:578-582](file:///c:/Users/Kushal/OneDrive/Documents/sem-3%20lab/ac/SnakeX/code/game.cpp#L578-L582)
* **Confidence Level**: C4 (Certain)
* **Description**: The snake and food instantiation and spawn logic are duplicated identically between the `Game` constructor and the `Game::restart` method.
* **Suggested Refactoring**: Extract the initialization logic into a common private setup/reset helper method.

---

## Couplers

### Inappropriate Intimacy
* **Location**: [game.cpp:430](file:///c:/Users/Kushal/OneDrive/Documents/sem-3%20lab/ac/SnakeX/code/game.cpp#L430), [game.cpp:520](file:///c:/Users/Kushal/OneDrive/Documents/sem-3%20lab/ac/SnakeX/code/game.cpp#L520), and [game.cpp:528-531](file:///c:/Users/Kushal/OneDrive/Documents/sem-3%20lab/ac/SnakeX/code/game.cpp#L528-L531)
* **Confidence Level**: C3 (High)
* **Description**: The `Game` class directly accesses the internal `deque<Position>` container of `Snake` via `snake->getBody()` to perform food spawning and game board rendering. This tightly couples `Game` to `Snake`'s internal storage choice.
* **Suggested Refactoring**: Encapsulate the lookup or rendering responsibility, or expose read-only iteration mechanisms.
