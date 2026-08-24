# Ubiquitous Language

## Game Entities and Environment

| Term | Definition | Aliases to avoid | In code |
| ---- | ---------- | ---------------- | ------- |
| **Snake** | The player-controlled serpent that moves and grows when consuming food. | Worm, Serpent, Player | `Snake` — `code/game.cpp:295` |
| **Food** | An item spawned on the board that the Snake consumes to grow and score. | Apple, Fruit | `Food` — `code/game.cpp:274`, `EMOJI_FOOD` — `code/game.cpp:258` |
| **Game Board** | The bounded grid area within which the Snake moves and Food is spawned. | Grid, Arena, Board | `GameBoard` — `code/game.cpp:345` |

## Game Mechanics and Lifecycle

| Term | Definition | Aliases to avoid | In code |
| ---- | ---------- | ---------------- | ------- |
| **Tick Delay** | The time interval in milliseconds between successive movements of the Snake. | Speed, Velocity, Sleep | `speedMs` — `code/game.cpp:412` ⚠ |
| **Collision** | An event where the Snake's head hits the board boundary or its own body, ending the game. | Crash, Death, Hit | `checkSelfCollision` — `code/game.cpp:337` |
| **Score** | The count of Food consumed by the Snake during the current run. | Points, Count | `score` — `code/game.cpp:410` |
| **High Score** | The maximum Score recorded across all runs, persisted in storage. | Best Score, Top Score | `highScore` — `code/game.cpp:410` |

## Relationships

- A **Snake** moves within the boundaries of the **Game Board**.
- A **Snake** grows and increases the **Score** by consuming **Food**.
- A **Collision** occurs if the **Snake** exits the **Game Board** boundaries or intersects its own body.
- The **Tick Delay** controls the physical speed of the **Snake** and decreases as more **Food** is consumed.

## Example dialogue

> **Dev:** "When the **Snake** consumes **Food**, does the **Tick Delay** decrease immediately?"
> **Domain expert:** "No, we only decrease the **Tick Delay** (speeding up the game) after the **Snake** consumes every 4 units of **Food**."
> **Dev:** "And does hitting the wall count as a **Collision**?"
> **Domain expert:** "Yes, any boundary violation or self-intersection is a **Collision**, which transitions the game to the Game Over state."

## Flagged ambiguities

- "Speed" vs "Delay": The variable is named `speedMs` but actually represents the time delay between frames (ticks). As the game gets faster, `speedMs` decreases. We recommend using **Tick Delay** to refer to the interval, and reserving **Speed** for gameplay speed description.
- "Apple" vs "Food" vs "Fruit": The code uses `Food` class and `EMOJI_FOOD` constant, but names the speed-up tracker `appleCount` and mentions "apples eaten". We canonicalize this as **Food**.

## Code drift

| Canonical term | Called in code | Location | Note |
| -------------- | -------------- | -------- | ---- |
| **Tick Delay** | `speedMs` | `code/game.cpp:412` | Variable represents delay/interval, not speed |
| **Food** | `appleCount` | `code/game.cpp:412` | Tracks speed-up based on Food consumed but refers to apples |
| **Food** | `EMOJI_FOOD` | `code/game.cpp:258` | Code defines constant with EMOJI prefix but refers to food |
