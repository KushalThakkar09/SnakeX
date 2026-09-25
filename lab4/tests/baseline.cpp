// The build script supplies verbatim components from lab4-base with #line markers.
#include "baseline_components.h"
#include "check.h"

void initialSnakeHasThreeCells() {
    Snake snake(5, 5);
    CHECK(snake.getBody().size() == 3);
    CHECK(snake.getHead() == Position(5, 5));
}

void movementContinuesAndRejectsReversal() {
    Snake snake(5, 5);
    snake.move();
    CHECK(snake.getHead() == Position(6, 5));
    snake.move();
    CHECK(snake.getHead() == Position(7, 5));
    snake.setDirection(LEFT);
    snake.move();
    CHECK(snake.getHead() == Position(8, 5));
}

void foodSamplesAvoidOccupiedCells() {
    // This samples the real RNG; it does not force any particular retry sequence.
    std::srand(643);
    const std::deque<Position> occupied{{5, 5}, {4, 5}, {3, 5}};
    Food food;
    for (int sample = 0; sample < 100; ++sample) {
        food.spawn(12, 10, occupied);
        const Position position = food.getPosition();
        CHECK(position.x > 0 && position.x < 11);
        CHECK(position.y > 0 && position.y < 9);
        for (const Position& cell : occupied) {
            CHECK(!(position == cell));
        }
    }
}

void boundaryPredicateRecognizesWalls() {
    GameBoard board(12, 10);
    CHECK(board.isInsideBoundaries(Position(1, 1)));
    CHECK(board.isInsideBoundaries(Position(10, 8)));
    CHECK(!board.isInsideBoundaries(Position(0, 5)));
    CHECK(!board.isInsideBoundaries(Position(11, 5)));
    CHECK(!board.isInsideBoundaries(Position(5, 0)));
    CHECK(!board.isInsideBoundaries(Position(5, 9)));
}

void selfCollisionPredicateRecognizesLoop() {
    Snake snake(5, 5);
    CHECK(!snake.checkSelfCollision());
    // Growth executes as arrangement, but its length effect is not asserted here.
    snake.grow();
    snake.move();
    snake.setDirection(DOWN);
    snake.grow();
    snake.move();
    snake.setDirection(LEFT);
    snake.grow();
    snake.move();
    snake.setDirection(UP);
    snake.move();
    CHECK(snake.checkSelfCollision());
}

int main() {
    int failures = 0;
    runTest("initial snake has three cells", initialSnakeHasThreeCells, failures);
    runTest("movement continues and rejects reversal", movementContinuesAndRejectsReversal, failures);
    runTest("food samples avoid occupied cells", foodSamplesAvoidOccupiedCells, failures);
    runTest("boundary predicate recognizes walls", boundaryPredicateRecognizesWalls, failures);
    runTest("self collision predicate recognizes loop", selfCollisionPredicateRecognizesLoop, failures);
    return failures == 0 ? 0 : 1;
}
