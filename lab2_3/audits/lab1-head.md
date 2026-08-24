# Code Smell Audit: Lab 1 Head (Commit 9bbd642)

This audit documents the code smells identified in the 2-player multiplayer version of the Snake game as implemented in Lab 1 (`9bbd642` state).

---

## Bloaters

### Primitive Obsession
* **Location**: [game.cpp:364](file:///c:/Users/Kushal/OneDrive/Documents/sem-3%20lab/ac/SnakeX/code/game.cpp#L364)
* **Confidence Level**: C3 (High)
* **Description**: Coordinates in `GameBoard::place` are still represented as raw `int x` and `int y` parameters rather than using the `Position` structure.
* **Status**: Left untouched from baseline.

### Duplicate Code (Introduced)
* **Location**: [game.cpp:440-444](file:///c:/Users/Kushal/OneDrive/Documents/sem-3%20lab/ac/SnakeX/code/game.cpp#L440-L444) and [game.cpp:627-631](file:///c:/Users/Kushal/OneDrive/Documents/sem-3%20lab/ac/SnakeX/code/game.cpp#L627-L631)
* **Confidence Level**: C4 (Certain)
* **Description**: The creation and initialization of `snake1` and `snake2` are duplicated verbatim between the `Game` constructor and the `Game::restart` method.
* **Status**: Introduced (expanded from the single-snake duplication in the baseline).

### Duplicate Code (Introduced)
* **Location**: [game.cpp:573-581](file:///c:/Users/Kushal/OneDrive/Documents/sem-3%20lab/ac/SnakeX/code/game.cpp#L573-L581)
* **Confidence Level**: C4 (Certain)
* **Description**: The rendering logic in `Game::render` contains duplicate loops for iterating and placing segments for `snake1` and `snake2` on the game board.
* **Status**: Introduced in Lab 1.

---

## Object-Orientation Abusers

### Switch Statement (If-Else Chain on Symbol Types)
* **Location**: [game.cpp:390-394](file:///c:/Users/Kushal/OneDrive/Documents/sem-3%20lab/ac/SnakeX/code/game.cpp#L390-L394)
* **Confidence Level**: C3 (High)
* **Description**: `GameBoard::render` still relies on an if-else chain to inspect cell strings for coloring.
* **Status**: Left untouched from baseline.

### Switch Statement (Direction Enum)
* **Location**: [game.cpp:323-329](file:///c:/Users/Kushal/OneDrive/Documents/sem-3%20lab/ac/SnakeX/code/game.cpp#L323-L329)
* **Confidence Level**: C2 (Medium)
* **Description**: `Snake::move` continues to use a `switch(current)` block for movement coordinates update.
* **Status**: Left untouched from baseline.

---

## Couplers

### Inappropriate Intimacy
* **Location**: [game.cpp:520](file:///c:/Users/Kushal/OneDrive/Documents/sem-3%20lab/ac/SnakeX/code/game.cpp#L520), [game.cpp:528](file:///c:/Users/Kushal/OneDrive/Documents/sem-3%20lab/ac/SnakeX/code/game.cpp#L528), and [game.cpp:573-579](file:///c:/Users/Kushal/OneDrive/Documents/sem-3%20lab/ac/SnakeX/code/game.cpp#L573-L579)
* **Confidence Level**: C3 (High)
* **Description**: The `Game` class directly accesses the internal deques of both `snake1` and `snake2` to check collisions and draw body cells on the board.
* **Status**: Left untouched/compounded from baseline (extended to both snakes).
