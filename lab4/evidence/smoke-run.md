# Actual game execution — 2026-09-25

Build: `g++ -std=c++17 -Wall -Wextra code/game.cpp -o snake.exe`.
Platform: Windows, MinGW GCC 6.3.0, interactive PTY. No compiler warnings or errors.
The binary built from commit-1 source was run in a separate scratch working directory.

Observed sequence:

1. Welcome screen appeared with saved high score 0 and the existing controls.
2. Sent `x` to start. The board rendered with a three-cell snake and food.
3. Without additional direction input, the snake advanced right until the wall.
4. Game-over screen appeared with final score 0 and R/Q choices.
5. Sent `r`; the board restarted with a three-cell snake and score 0.
6. Sent `q`; the thank-you message appeared and the process returned exit code 0.

This checks that the refactored executable starts and runs. It is not a comparison
against `lab4-base`, which does not compile, and does not establish visual equivalence
for unexercised paths such as pause or food consumption. Eating and scoring are covered
separately by deterministic tests of the shared production update operation.
