# Lab 2_3 — Group SnakeX

---

## 1. Tool and install route — [3]

| | |
|---|---|
| Agent used for run 2 | Gemini 3.5 Flash |
| `ubiquitous-language` install route | project `.agents/skills` |
| `refactoring/` pack install route | project `.agents/skills` |

The Antigravity agent workspace customization path (`.agents/skills/`) was used to load both the ubiquitous language and refactoring skills.

---

## 2. What I changed in the glossary — [4]

The generated file is at `lab2_3/UBIQUITOUS_LANGUAGE.md`. We modified it to:
- Canonicalize **Food** as the single term for apples, fruit, and food items.
- Formulate **Tick Delay** to replace the misnamed `speedMs` variable (which stores a time interval/delay rather than speed).
- Formulate explicit **Collision** scenarios (wall-collision, self-collision, and inter-snake collision) to define the boundaries of game over states.

---

## 3. Smell delta — [6]

Reports: `lab2_3/audits/main.md` (the code as you received it) and `lab2_3/audits/lab1-head.md` (after your Lab-1 PR).

| | count | representative site (`file:line`) |
|---|---|---|
| Smells my Lab-1 PR **introduced** | 2 | `code/game.cpp:573` |
| Smells my Lab-1 PR **left untouched** | 4 | `code/game.cpp:364` |
| Smells my Lab-1 PR **removed** | 0 | N/A |

---

## 4. Rejected candidates — [6]

At least three things the agent reported that are *not* real findings on this codebase.

| smell reported | `file:line` | why it does not hold |
|---|---|---|
| Large Class | `code/game.cpp:41` | The `Terminal` class cohesively groups console screen/cursor settings and non-blocking input utilities. |
| Switch Statement | `code/game.cpp:323` | The `switch` statement on `Direction` is highly readable and handles a fixed set of four directions. |
| Dead Code | `code/game.cpp:70` | This is POSIX-specific terminal mode configuration code that is skipped on Windows but required for macOS/Linux. |

---

## 5. Commit map — [7]

Run `lab2_3/check-lab2_3.sh` and paste the table it prints.

| # | sha | subject | what it is |
|---|---|---|---|
| 1 | 613c85e | docs: create ubiquitous language glossary | glossary |
| 2 | 7218005 | docs: generate baseline code smell report for main branch | smell report |
| 3 | 84daece | refactor: parameterize snake count with NUM_PLAYERS constant | **the refactor, alone** |
| 4 | a174c70 | feat: enable 2-player local multiplayer by changing NUM_PLAYERS to 2 | **the feature, alone** |

---

## 6. Two-run measurement — [4]

Run 1 is your Lab-1 branch — the numbers you already reported. Run 2 is commit 4 alone.

| | Run 1 (Lab 1) | Run 2 (commit 4) |
|---|---|---|
| Smells introduced | 2 | 0 |
| Lines changed, `git diff --shortstat -w` | 3 files changed, 159 insertions(+), 48 deletions(-) | 2 files changed, 1 insertion(+), 1 deletion(-) |
| Lines changed, **raw** (no `-w`) | 3 files changed, 159 insertions(+), 48 deletions(-) | 2 files changed, 1 insertion(+), 1 deletion(-) |
| Functions reached | 9 | 0 |
| Prompts to working code | 2 | 1 |
| Wall-clock time | 5 mins | 1 min |

Commit 3 (the refactor) on its own: 2 files changed, 162 insertions(+), 54 deletions(-) `-w`, 2 files changed, 168 insertions(+), 60 deletions(-) raw.

---

## 7. Analysis Q1–Q2 — [5]

**Q1. Which smell did commit 3 actually fix?**

Commit 3 fixed the **Duplicate Code** smell by extracting the setup of the snakes and food into the helper method `setupSnakesAndFood()`. It also fixed the **Inappropriate Intimacy** smell by introducing parameterized loops for snake input, movement, rendering, and collision checking using a `vector<Snake*>` instead of directly accessing individual snake instances. In the baseline code, modifying coordinates or adding features required duplicate logic for each snake, whereas now the game loop dynamically scales with `NUM_PLAYERS`.

**Q2. Compare commit 4 to your Lab-1 diff.**

In Lab 1, adding multiplayer required writing 160 lines of code across 9 functions because controls, collision detection, and rendering loops were manually duplicated for the second player. In commit 4, the exact same feature was enabled by changing a single line of code (`const int NUM_PLAYERS = 2;`). The cost of adding the feature dropped to zero because the design-first refactoring step in commit 3 had already introduced the necessary structural seams.

---

## 8. Analysis Q3–Q4 — [5]

**Q3. Go back through your Lab-1 `LLM-LOG.md`. Did the assistant ever suggest restructuring before adding the feature?**

No, the assistant directly proposed modifying `code/game.cpp` to add a second snake pointer and duplicate all logic without restructuring the code first. To make the agent suggest restructuring first, the prompt would have needed to explicitly separate concerns: "Analyze the codebase for single-player hardcoding, refactor it first to support an arbitrary player count, verify it works, and only then enable the second player."

**Q4. How do you know commit 3 did not change behaviour?**

We played the game manually to confirm single-player behavior holds. However, manual playing is error-prone and does not cover edge cases. To truly know, we would have needed automated unit and integration tests covering game state transitions, tick speeds, and high score serialization, enabling us to run a regression test suite before and after the refactor.

---

## If you did not finish
