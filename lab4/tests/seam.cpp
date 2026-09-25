// Reuse the actual translation unit without launching its interactive entry point.
#define main snakexInteractiveMain
#include "../../code/game.cpp"
#undef main
#include "check.h"

class ScriptedFood : public FoodSource {
    std::vector<Position> positions;
    std::size_t current = 0;
public:
    explicit ScriptedFood(std::initializer_list<Position> answers) : positions(answers) {}
    Position getPosition() const override { return positions.at(current); }
    void spawn(int, int, const std::deque<Position>&) override {
        // Canned next answer, not a simulation of random placement.
        positions.at(current + 1); // Fail if the fixture has no replacement food.
        ++current;
    }
};

void eatingIncrementsScoreExactlyOnce() {
    Snake snake(5, 5);
    GameBoard board(12, 12);
    ScriptedFood food{{6, 5}, {1, 1}};
    GameProgress progress;
    progress.score = 7;
    advanceGame(snake, board, food, progress);
    CHECK(progress.score == 8);
    advanceGame(snake, board, food, progress);
    CHECK(progress.score == 8);
}

void eatingGrowsByOneOnTheFollowingMove() {
    Snake snake(5, 5);
    GameBoard board(12, 12);
    ScriptedFood food{{6, 5}, {1, 1}};
    GameProgress progress;
    const auto originalLength = snake.getBody().size();
    advanceGame(snake, board, food, progress);
    CHECK(snake.getBody().size() == originalLength);
    advanceGame(snake, board, food, progress);
    CHECK(snake.getBody().size() == originalLength + 1);
    advanceGame(snake, board, food, progress);
    CHECK(snake.getBody().size() == originalLength + 1);
}

void noFoodPreservesProgressAndLength() {
    Snake snake(5, 5);
    GameBoard board(12, 12);
    ScriptedFood food{{1, 1}};
    GameProgress progress;
    progress.score = 7;
    progress.highScore = 10;
    progress.appleCount = 2;
    progress.speedMs = 100;
    advanceGame(snake, board, food, progress);
    CHECK(progress.score == 7);
    CHECK(progress.highScore == 10);
    CHECK(progress.appleCount == 2);
    CHECK(progress.speedMs == 100);
    CHECK(!progress.gameOver);
    CHECK(snake.getBody().size() == 3);
    CHECK(snake.getHead() == Position(6, 5));
}

void aNewRecordUpdatesHighScore() {
    Snake snake(5, 5);
    GameBoard board(12, 12);
    ScriptedFood food{{6, 5}, {1, 1}};
    GameProgress progress;
    progress.score = 7;
    progress.highScore = 7;
    advanceGame(snake, board, food, progress);
    CHECK(progress.highScore == 8);
}

void existingHighScoreSurvivesEating() {
    Snake snake(5, 5);
    GameBoard board(12, 12);
    ScriptedFood food{{6, 5}, {1, 1}};
    GameProgress progress;
    progress.highScore = 20;
    advanceGame(snake, board, food, progress);
    CHECK(progress.highScore == 20);
}

void fourthAppleReducesDelayAndResetsCounter() {
    Snake snake(5, 5);
    GameBoard board(12, 12);
    ScriptedFood food{{6, 5}, {1, 1}};
    GameProgress progress;
    progress.appleCount = 3;
    advanceGame(snake, board, food, progress);
    CHECK(progress.speedMs == 132);
    CHECK(progress.appleCount == 0);
}

void earlierAppleDoesNotReduceDelay() {
    Snake snake(5, 5);
    GameBoard board(12, 12);
    ScriptedFood food{{6, 5}, {1, 1}};
    GameProgress progress;
    progress.appleCount = 2;
    advanceGame(snake, board, food, progress);
    CHECK(progress.speedMs == 140);
    CHECK(progress.appleCount == 3);
}

void delayNeverFallsBelowThirtyMilliseconds() {
    Snake snake(5, 5);
    GameBoard board(12, 12);
    ScriptedFood food{{6, 5}, {1, 1}};
    GameProgress progress;
    progress.appleCount = 3;
    progress.speedMs = 34;
    advanceGame(snake, board, food, progress);
    CHECK(progress.speedMs == 30);
}

void wallCollisionEndsGameBeforeScoring() {
    Snake snake(10, 5);
    GameBoard board(12, 12);
    ScriptedFood food{{11, 5}};
    GameProgress progress;
    progress.score = 7;
    advanceGame(snake, board, food, progress);
    CHECK(progress.gameOver);
    CHECK(progress.score == 7);
}

void selfCollisionEndsGameBeforeScoring() {
    Snake snake(5, 5);
    snake.grow(); snake.move();
    snake.setDirection(DOWN); snake.grow(); snake.move();
    snake.setDirection(LEFT); snake.grow(); snake.move();
    snake.setDirection(UP);
    GameBoard board(12, 12);
    ScriptedFood food{{5, 5}};
    GameProgress progress;
    progress.score = 7;
    advanceGame(snake, board, food, progress);
    CHECK(progress.gameOver);
    CHECK(progress.score == 7);
}

int main() {
    int failures = 0;
    runTest("eating increments score exactly once", eatingIncrementsScoreExactlyOnce, failures);
    runTest("eating grows by one on following move", eatingGrowsByOneOnTheFollowingMove, failures);
    runTest("no food preserves progress and length", noFoodPreservesProgressAndLength, failures);
    runTest("new record updates high score", aNewRecordUpdatesHighScore, failures);
    runTest("existing high score survives eating", existingHighScoreSurvivesEating, failures);
    runTest("fourth apple reduces delay and resets counter", fourthAppleReducesDelayAndResetsCounter, failures);
    runTest("earlier apple does not reduce delay", earlierAppleDoesNotReduceDelay, failures);
    runTest("delay never falls below 30 ms", delayNeverFallsBelowThirtyMilliseconds, failures);
    runTest("wall collision ends game before scoring", wallCollisionEndsGameBeforeScoring, failures);
    runTest("self collision ends game before scoring", selfCollisionEndsGameBeforeScoring, failures);
    return failures == 0 ? 0 : 1;
}
