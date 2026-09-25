# Lab 4: local submission

PR title: **Lab 4: eating food increases score and grows the snake**

This branch is prepared for `KushalThakkar09/SnakeX`, with exactly two commits after `lab4-base`. It has not been pushed and no PR has been opened, per the request to prepare files locally.

Read `REPORT.md` before submitting. The supplied baseline fails to compile; the report discloses the component-only Part B/C workaround and the declaration repairs in commit 1. Whole-game baseline coverage and a playable before/after comparison cannot honestly be claimed for that base.

## Run tests

Requirements: Git, Windows PowerShell 5.1 or PowerShell 7, and matching `g++`/`gcov` on `PATH` (validated with MinGW 6.3.0). No gtest download is required.

From the repository root:

```powershell
powershell -ExecutionPolicy Bypass -File lab4/run-tests.ps1 -Suite all
powershell -ExecutionPolicy Bypass -File lab4/check-mutations.ps1
```

The first command runs 5 baseline component tests and 10 seam tests and writes gcov results below `lab4/build/`. The second compiles six isolated source mutations under that ignored directory and requires each to fail a behavioral assertion. It never edits `code/game.cpp`.

To build and play the real game:

```powershell
g++ -std=c++17 -Wall -Wextra code/game.cpp -o lab4/build/snake.exe
./lab4/build/snake.exe
```

Play from a scratch directory if you do not want to change the repository's `scores.txt`. The unit tests do not instantiate `Game` or access that file.

On a POSIX machine with GCC, the seam suite can also be built directly (not exercised in this Windows validation):

```sh
g++ -std=c++17 --coverage -O0 -g lab4/tests/seam.cpp -o /tmp/snakex-tests
/tmp/snakex-tests
```

## Submit from the Git bundle

From the directory containing `SnakeX-Lab4.bundle`, choose a new destination directory:

```powershell
git clone -b lab4 ./SnakeX-Lab4.bundle SnakeX-Lab4
cd SnakeX-Lab4
git remote set-url origin https://github.com/KushalThakkar09/SnakeX.git
git log --oneline lab4-base..lab4
git diff --name-only lab4~1 lab4 --
git fetch origin main
```

Check that the two commits are the seam then tests and that the second diff contains only `lab4/` files. If the remote main has advanced from the pinned base, review that change before submitting; do not move the base tag silently.

When signed in with permission to push to the repository:

```powershell
git push origin refs/tags/lab4-base refs/heads/lab4
```

Open [the pull request comparison](https://github.com/KushalThakkar09/SnakeX/compare/main...lab4?expand=1), use the title above, and paste the full `lab4/REPORT.md` as the description. Replace its local-only PR status with the created PR URL in the PR description. Do not create a third commit just to record the PR number. If a `lab4` branch or `lab4-base` tag already exists remotely, inspect it instead of force-pushing.

## Contents and history

- `tests/baseline.cpp`: assertions run against verbatim tagged components.
- `tests/seam.cpp`: deterministic tests using the actual refactored production code.
- `evidence/`: build failure, test output, coverage, mutation results and actual gameplay observations.
- `PART-A.md`: rules recorded before opening C++ source.
- `REPORT.md`: six assignment sections, including limitations and test smells.
- `WORKLOG.md`: how the local work was produced.

Baseline, first commit and second commit can be inspected as `lab4-base`, `lab4~1`, and `lab4`, respectively. The second commit contains the baseline tests as well as the seam tests so the required two-commit history remains intact; baseline evidence was gathered before the source edit.
