# Lab 4 — Group SnakeX

| | |
|---|---|
| Repository | https://github.com/KushalThakkar09/SnakeX (repository supplied for this task) |
| Base tag | `lab4-base` at `d4db6117edc19610472ad3df2e9cdeba840146e1` |
| Pull request | Prepared locally, as requested; open `lab4` → `main` after pushing and paste this report into the description. |

**Baseline qualification:** the supplied default branch does not compile. `Food::spawn` has an undeclared multiplayer overload and an unused setup method references an undeclared `snakes` member. Part B therefore uses a build target that selects *verbatim* component definitions from the tagged source; it does not repair or rewrite that source. The component coverage below is explicitly scoped, not whole-game coverage. Commit 1 includes the two missing declarations as well as the seam. A literal playable before/after comparison against the broken base is unavailable; this deviation from the assignment is disclosed rather than presented as verified equivalence.

---

## 1. Five rules — [5]

Recorded from the README before opening the C++ source, in `lab4/PART-A.md`. These are behavioral expectations, not claims that every expectation already has an integration test.

| # | Rule |
|---|---|
| 1 | A new game starts with a snake three cells long. |
| 2 | The snake continues moving in its current direction. |
| 3 | Eating food grows the snake by one cell and increases the score. |
| 4 | Hitting a wall or the snake's own body ends the game. |
| 5 | Food never spawns on a cell occupied by the snake. |

No rule required reading the implementation to formulate. Later inspection clarified rule 3: score rises by exactly one during the eating tick, while length increases on the following move. That existing timing is preserved. The README's single-player description also differs from the partial multiplayer additions in the source; this lab does not finish that feature.

---

## 2. What you could test, and what stopped you — [10]

No production source was changed for this part. Every `code/game.cpp:line` in this section refers to `lab4-base`.

| # | Rule | Test written? | Blocking dependency (`file:line` + what it is) |
|---|---|---|---|
| 1 | Initial length is three | Yes, at the `Snake` constructor boundary: `initialSnakeHasThreeCells`. | `code/game.cpp:305`–`code/game.cpp:309` constructs three cells and `code/game.cpp:311` exposes the body. Actual `Game` startup is not exercised: `code/game.cpp:432` embeds a concrete terminal and `code/game.cpp:467` reads the score file. |
| 2 | Movement continues | Yes: `movementContinuesAndRejectsReversal` checks two successive rightward moves without new input. | `code/game.cpp:323` exposes `move()` and `code/game.cpp:312` exposes the head; there is no input/sleep dependency at this boundary. Keyboard polling at `code/game.cpp:486` and the timed loop at `code/game.cpp:641` are outside this test. |
| 3 | Eating grows and scores | No complete behavioral test; growth executes only as setup for a collision predicate test. | Eating is wired inside `Game::update`, `code/game.cpp:556`–`code/game.cpp:565`. `code/game.cpp:435` fixes the collaborator to concrete `Food*`; `code/game.cpp:290`–`code/game.cpp:291` chooses random positions; `code/game.cpp:434` and `code/game.cpp:436` hide the snake and score. There is no terminal-free tick boundary with supplied food and observable progress. |
| 4 | Collision ends the game | Partial only: boundary and self-collision *predicates* are checked, not `gameOver`. | `code/game.cpp:395` and `code/game.cpp:341` provide predicates, but the actual end-state assignment is `code/game.cpp:552`; its state is private at `code/game.cpp:437`. Running the interactive path also reaches keyboard polling and sleep at `code/game.cpp:598`–`code/game.cpp:610`. |
| 5 | Food avoids the snake | Yes, sampled invariant: `foodSamplesAvoidOccupiedCells` checks 100 placements against every occupied cell. | `code/game.cpp:287` accepts an occupied body and `code/game.cpp:284` returns the result. Direct `rand()` calls at `code/game.cpp:290`–`code/game.cpp:291` prevent supplying a scripted retry sequence in this harness, but do not prevent assertions about each returned cell. The test is sampling, not an exhaustive proof. |

> **Rules testable without modifying the source: 3 / 5 at component boundaries.**

There are five baseline test functions, but only rules 1, 2 and 5 have direct rule assertions. The two collision tests do not count as a verified game-over rule. No complete-game baseline test can run because the original translation unit fails to compile.

The shared build blocker is `code/game.cpp:356`, defining a `Food::spawn` overload absent from the class at `code/game.cpp:278`, plus `code/game.cpp:443`, referring to undeclared `snakes`. The unchanged compiler errors are saved in `lab4/evidence/base-build.txt`.

`lab4/run-tests.ps1 -Suite baseline` reads the tag with `git show`, selects exact original lines 1–354 and 371–428, and adds `#line` directives. This includes the original `Position`, `Food`, `Snake` and `GameBoard` definitions, skips the broken unused multiplayer definition, and excludes interactive `Game`/`main`. It neither copies an independently reimplemented algorithm into tests nor claims to test the omitted code. Generated files stay in ignored `lab4/build/`.

---

## 3. Coverage, and what it missed — [6]

Measured on Windows with MinGW GCC/gcov 6.3.0, using `--coverage -O0 -g`, after clearing earlier `.gcda` counters.

| | |
|---|---|
| Line coverage | **82.05%** of 78 instrumented production component lines (64 executed) |
| Branch coverage | **72.73%** of 110 branches executed (80); **57.27%** taken at least once (63 outcomes) |
| Command used | `powershell -ExecutionPolicy Bypass -File lab4/run-tests.ps1 -Suite baseline` |
| Compiler / coverage commands | `g++ -std=c++17 --coverage -O0 -g -Wall -Wextra -I. -c <baseline.cpp> -o tests.o`; `g++ --coverage tests.o -o tests.exe`; `./tests.exe`; `gcov -b -c tests.gcno` |
| Measurement scope | Tagged production components selected by the baseline build target; test code and standard-library coverage are not included in the percentages above. Full-game baseline coverage is unavailable because it does not build. |

**One rule that is executed by the suite but not verified by it:**

| | |
|---|---|
| Rule | Rule 3, specifically the growth behavior; eating-to-score integration does not run in Part B. |
| Line that runs | At `lab4-base`, `code/game.cpp:339` (`grow`) executes 3 times, and `code/game.cpp:336` (consume the growth flag while retaining the tail) executes 3 times. |
| The assertion that is missing | In the baseline collision arrangement, save the length before `grow(); move();` and assert `lengthAfter == lengthBefore + 1`; a complete rule-3 test must also arrange food at the next head and assert `scoreAfter == scoreBefore + 1`. |

`selfCollisionPredicateRecognizesLoop` uses growth to arrange an overlapping body, but only checks collision. It may incidentally detect some broken growth implementations; it does not establish the exact one-cell growth contract. Executing the growth lines is therefore not the same as verifying that contract. The later seam suite adds the explicit length and score assertions.

Raw summaries and line counts: `lab4/evidence/baseline-coverage.txt` and `lab4/evidence/baseline-game.cpp.gcov`. Post-seam tests compile the **complete** production translation unit and obtain 19.34% line coverage (70/362), 18.40% branches executed (92/500), and 13.60% branch outcomes taken (68/500). These percentages have a different denominator and must not be compared as a coverage regression. They deliberately leave the console, persistence and interactive loop unexecuted. `advanceGame` itself is called 13 times and gcov reports 100% of its blocks executed.

Machine-specific repository paths in the saved evidence are normalized to `<repo>`; counters, source text and line numbers are unchanged.

---

## 4. The seam — [10]

| | |
|---|---|
| Rule made testable | Rule 3: eating increments score by exactly one and schedules exactly one extra body cell on the next move. |
| Commit 1 (seam) | `8e15f73` — `refactor: expose game tick through a food collaborator seam` |
| Commit 2 (test) | `lab4` (the second commit after `lab4-base`) — `test: verify eating behavior and record lab 4 evidence`; changes only `lab4/` test/build/report files. |
| Seam kind | **Object seam** (`FoodSource&`). Renaming `main` inside the test translation unit is only the harness mechanism for including the existing monolithic source. |
| Enabling point | `code/game.cpp:451`: the caller supplies `FoodSource&` to `advanceGame`. Production supplies its real `Food` from `Game::update` at `code/game.cpp:592`; tests supply `ScriptedFood`. |
| What production code gave up | Exclusive ownership of tick orchestration and private progress representation: the update operation and `GameProgress` now form a callable API. `Food` also gains virtual dispatch and a virtual destructor, and callers of the new API must provide coherent state and a valid food collaborator. This is a small but real encapsulation/ABI cost, not “nothing.” |

The original tick order is preserved: move, reject collision, check food, schedule growth, increment score/counter, adjust delay after four apples with a 30 ms floor, update high score, respawn. Default score/high score/counter/game-over state and 140 ms delay remain unchanged. Restart still retains high score and resets the same fields. Production food still uses the original `rand()` algorithm.

A mechanical comparison against `lab4-base` also confirmed that the extracted tick body is identical after replacing pointer member access with references, qualifying the moved progress fields, and ignoring whitespace (`lab4/evidence/source-audit.txt`). This supports the algorithm-preservation claim; it is not a substitute for the unavailable baseline gameplay comparison.

Commit 1 also supplies the two missing declarations described in section 2: a declaration matching the existing multiplayer `Food::spawn` definition, and vector storage used by the already-present but uncalled setup helper. No call to that helper is added and no multiplayer behavior is activated. This repairs compilation, so the commit is not a strictly seam-only diff even though the intended gameplay algorithm is unchanged.

Validation: the normal game builds with `g++ -std=c++17 -Wall -Wextra code/game.cpp -o snake.exe`. The compiled game was actually run in a Windows terminal: start screen, movement, wall game-over, restart and quit were observed; exit status was 0. The broken baseline cannot be run for a direct visual comparison, and no such comparison is claimed. See `lab4/evidence/smoke-run.md`.

The 10 post-seam tests cover exact scoring, delayed one-cell growth, a non-eating tick, both high-score branches, apple threshold timing, the delay floor, and wall/self collision before scoring. All pass. Six independently compiled mutants (missing/doubled scoring, missing growth, missing game-over, missing delay floor, missing high-score update) all fail behavioral assertions; compilation failures are not counted as killed mutants.

---

## 5. The double — [4]

| | |
|---|---|
| What you passed through the seam | **Stub**: `ScriptedFood`, `lab4/tests/seam.cpp:7`. |
| The method under test | `advanceGame`, the extracted operation called by `Game::update`. |

The collaborator is asked “where is the food?” through `getPosition()`, and the stub returns positions specified by the test; `spawn()` merely advances to the next canned answer without simulating random placement. Tests assert resulting score, length and game-over state rather than recorded calls or an expected interaction protocol, so this is a stub, not a spy or mock.

---

## 6. Two smells in your own tests — [5]

| | Smell | `file:line` | One-line fix |
|---|---|---|---|
| 1 | Eager test | `lab4/tests/baseline.cpp:11` | Split continuous movement and reversal rejection into separate tests, each with its own act and assertions. |
| 2 | Assertion roulette | `lab4/tests/baseline.cpp:33` | Add the sample number, generated position and occupied cell to failure diagnostics inside the placement loop. |

These are real limitations of this suite, reviewed against `lab4/SMELLS.md`. The assertion helper reports test name, expression and line, but a failed loop assertion still does not identify which of 100 samples and three occupied cells failed. Both findings are left visible for the assignment; neither is being passed off as a test that asserts nothing.
