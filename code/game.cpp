// Simple Snake Game - Cross-Platform Terminal Version (Improved rendering: no flicker)
// Auto-adapts graphics for Windows (ASCII) or Linux/macOS (Emoji)
// Compile: g++ -std=c++17 snake.cpp -o snake

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <deque>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>

    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
    #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
    #endif
#else
    // POSIX
    #include <termios.h>
    #include <unistd.h>
    #include <sys/ioctl.h>
    #include <sys/select.h>
#endif

using namespace std;

// ANSI colors
#define RED    "\033[31m"
#define GREEN  "\033[32m"
#define YELLOW "\033[33m"
#define CYAN   "\033[36m"
#define RESET  "\033[0m"

// ======================================================
// Cross-platform Terminal abstraction
// ======================================================
class Terminal {
private:
#ifdef _WIN32
    HANDLE hStdin;
    HANDLE hStdout;
    CONSOLE_CURSOR_INFO originalCursorInfo;
    bool cursorInfoSaved = false;
    deque<char> pendingChars;
#else
    struct termios original;
#endif

public:
    Terminal() {
#ifdef _WIN32
        hStdin = GetStdHandle(STD_INPUT_HANDLE);
        hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

        CONSOLE_CURSOR_INFO cinfo;
        if (GetConsoleCursorInfo(hStdout, &cinfo)) {
            originalCursorInfo = cinfo;
            cursorInfoSaved = true;
        }

        DWORD mode = 0;
        if (GetConsoleMode(hStdout, &mode)) {
            SetConsoleMode(hStdout, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        }
#else
        tcgetattr(STDIN_FILENO, &original);
        struct termios raw = original;
        raw.c_lflag &= ~(ICANON | ECHO);
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
#endif
    }

    ~Terminal() {
#ifdef _WIN32
        if (cursorInfoSaved) {
            SetConsoleCursorInfo(hStdout, &originalCursorInfo);
        }
#else
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
        showCursor();
#endif
    }

    void hideCursor() {
#ifdef _WIN32
        CONSOLE_CURSOR_INFO cci;
        if (GetConsoleCursorInfo(hStdout, &cci)) {
            cci.bVisible = FALSE;
            SetConsoleCursorInfo(hStdout, &cci);
        }
#else
        cout << "\033[?25l" << flush;
#endif
    }

    void showCursor() {
#ifdef _WIN32
        CONSOLE_CURSOR_INFO cci;
        if (GetConsoleCursorInfo(hStdout, &cci)) {
            cci.bVisible = TRUE;
            SetConsoleCursorInfo(hStdout, &cci);
        }
#else
        cout << "\033[?25h" << flush;
#endif
    }

    void clearScreen() {
#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        DWORD cellCount;
        DWORD count;
        COORD homeCoords = {0, 0};

        if (!GetConsoleScreenBufferInfo(hStdout, &csbi)) {
            system("cls");
            return;
        }

        cellCount = csbi.dwSize.X * csbi.dwSize.Y;
        FillConsoleOutputCharacter(hStdout, ' ', cellCount, homeCoords, &count);
        FillConsoleOutputAttribute(hStdout, csbi.wAttributes, cellCount, homeCoords, &count);
        SetConsoleCursorPosition(hStdout, homeCoords);
#else
        cout << "\033[2J\033[H" << flush;
#endif
    }

    void moveCursor(int x, int y) {
#ifdef _WIN32
        COORD pos;
        pos.X = (SHORT)(max(0, x - 1));
        pos.Y = (SHORT)(max(0, y - 1));
        SetConsoleCursorPosition(hStdout, pos);
#else
        cout << "\033[" << y << ";" << x << "H" << flush;
#endif
    }

    bool kbhit() {
#ifdef _WIN32
        if (!pendingChars.empty()) return true;
        return _kbhit() != 0;
#else
        struct timeval tv = {0, 0};
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        return select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv) == 1;
#endif
    }

    char getch() {
#ifdef _WIN32
        if (!pendingChars.empty()) {
            char c = pendingChars.front();
            pendingChars.pop_front();
            return c;
        }

        if (!_kbhit()) return 0;
        int c = _getch();
        if (c == 0 || c == 224) {
            int code = _getch();
            if (code == 72) { // up
                pendingChars.push_back(27); pendingChars.push_back('['); pendingChars.push_back('A');
            } else if (code == 80) { // down
                pendingChars.push_back(27); pendingChars.push_back('['); pendingChars.push_back('B');
            } else if (code == 77) { // right
                pendingChars.push_back(27); pendingChars.push_back('['); pendingChars.push_back('C');
            } else if (code == 75) { // left
                pendingChars.push_back(27); pendingChars.push_back('['); pendingChars.push_back('D');
            }
            char ch = pendingChars.front();
            pendingChars.pop_front();
            return ch;
        } else {
            return (char)c;
        }
#else
        char c = 0;
        ssize_t r = read(STDIN_FILENO, &c, 1);
        if (r <= 0) return 0;
        return c;
#endif
    }

    void getSize(int& width, int& height) {
#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(hStdout, &csbi)) {
            int cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
            int rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
            width = cols;
            height = rows;
            return;
        }
        width = 80;
        height = 25;
#else
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        width = w.ws_col;
        height = w.ws_row;
#endif
    }

    void sleep(int ms) {
#ifdef _WIN32
        ::Sleep(ms);
#else
        usleep(ms * 1000);
#endif
    }
};

// ======================================================
// Game Data Structures
// ======================================================
struct ScoreData { int previousScore; int highScore; };

ScoreData loadScores(const string &filename = "scores.txt") {
    ScoreData s = {0, 0};
    ifstream in(filename);
    if (in.is_open()) {
        in >> s.previousScore >> s.highScore;
        in.close();
    }
    return s;
}

void saveScores(const ScoreData &s, const string &filename = "scores.txt") {
    ofstream out(filename, ios::trunc);
    if (out.is_open()) {
        out << s.previousScore << " " << s.highScore << "\n";
        out.close();
    }
}

struct Position {
    int x, y;
    Position(int _x = 0, int _y = 0) : x(_x), y(_y) {}
    bool operator==(const Position& o) const { return x == o.x && y == o.y; }
};

enum Direction { UP, DOWN, LEFT, RIGHT, NONE };

// ======================================================
// Symbol Configuration (Platform-aware)
// ======================================================
#ifdef _WIN32 // Windows: ASCII mode
const string EMOJI_FOOD = "O";
const string EMOJI_SNAKE_HEAD = "@";
const string EMOJI_SNAKE_BODY = "o";
const string BORDER_CELL = "##";
const string EMPTY_CELL = "  ";
#else // Linux/macOS: Emoji mode
const string EMOJI_FOOD = "🍎";
const string EMOJI_SNAKE_HEAD = "🐍";
const string EMOJI_SNAKE_BODY = "🟩";
const string BORDER_CELL = "██";
const string EMPTY_CELL = "  ";
#endif

// ======================================================
// Food, Snake, GameBoard, and Game classes
// ======================================================
const int NUM_PLAYERS = 2;

class Snake;

class Food {
private:
    Position pos;
    string symbol;
public:
    Food() : symbol(EMOJI_FOOD) {}
    Position getPosition() const { return pos; }
    string getSymbol() const { return symbol; }
    void spawn(int maxX, int maxY, const vector<Snake*>& snakes);
};

class Snake {
private:
    deque<Position> body;
    Direction current, next;
    bool growing;
public:
    Snake(int startX, int startY, Direction startDir = RIGHT)
        : current(startDir), next(startDir), growing(false) {
        body.push_back(Position(startX, startY));
        if (startDir == RIGHT) {
            body.push_back(Position(startX - 1, startY));
            body.push_back(Position(startX - 2, startY));
        } else if (startDir == LEFT) {
            body.push_back(Position(startX + 1, startY));
            body.push_back(Position(startX + 2, startY));
        } else if (startDir == DOWN) {
            body.push_back(Position(startX, startY - 1));
            body.push_back(Position(startX, startY - 2));
        } else { // UP
            body.push_back(Position(startX, startY + 1));
            body.push_back(Position(startX, startY + 2));
        }
    }
    const deque<Position>& getBody() const { return body; }
    Position getHead() const { return body.front(); }
    string getHeadSymbol() const { return EMOJI_SNAKE_HEAD; }
    string getBodySymbol() const { return EMOJI_SNAKE_BODY; }

    void setDirection(Direction d) {
        if ((d == UP && current == DOWN) || (d == DOWN && current == UP) ||
            (d == LEFT && current == RIGHT) || (d == RIGHT && current == LEFT))
            return;
        next = d;
    }

    void move() {
        current = next;
        Position head = getHead();
        Position newHead = head;
        switch (current) {
            case UP: newHead.y--; break;
            case DOWN: newHead.y++; break;
            case LEFT: newHead.x--; break;
            case RIGHT: newHead.x++; break;
            case NONE: return;
        }
        body.push_front(newHead);
        if (!growing) body.pop_back();
        else growing = false;
    }

    void grow() { growing = true; }

    bool checkSelfCollision() const {
        Position h = getHead();
        for (size_t i = 1; i < body.size(); ++i)
            if (h == body[i]) return true;
        return false;
    }
};

void Food::spawn(int maxX, int maxY, const vector<Snake*>& snakes) {
    bool valid = false;
    while (!valid) {
        pos.x = rand() % (maxX - 2) + 1;
        pos.y = rand() % (maxY - 2) + 1;
        valid = true;
        for (const auto &s : snakes) {
            for (const auto &bodyPart : s->getBody()) {
                if (pos == bodyPart) { valid = false; break; }
            }
            if (!valid) break;
        }
    }
}

class GameBoard {
private:
    int width, height;
    vector<vector<string>> grid;
public:
    GameBoard(int w, int h) : width(w), height(h) {
        grid.assign(height, vector<string>(width, EMPTY_CELL));
        clear();
    }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    void clear() {
        for (int y = 0; y < height; ++y)
            for (int x = 0; x < width; ++x)
                grid[y][x] = (y == 0 || y == height-1 || x == 0 || x == width-1)
                              ? BORDER_CELL : EMPTY_CELL;
    }

    void place(int x, int y, const string &sym) {
        if (x >= 0 && x < width && y >= 0 && y < height)
            grid[y][x] = sym;
    }

    bool isInsideBoundaries(const Position& p) const {
        return p.x > 0 && p.x < width - 1 && p.y > 0 && p.y < height - 1;
    }

    // New render: build whole frame into stringstream and print once
    void render(Terminal& term, const vector<int>& scores, int highScore, int prevScore) const {
        // Move cursor to top-left once and overwrite
        term.moveCursor(1, 1);

        ostringstream out;
        out << CYAN << "SNAKE GAME  " << RESET;
        if (scores.size() == 1) {
            out << " | Score: " << GREEN << scores[0] << RESET;
        } else {
            out << " | P1 Score: " << GREEN << scores[0] << RESET
                << " | P2 Score: " << GREEN << scores[1] << RESET;
        }
        out << " | Prev: " << YELLOW << prevScore << RESET
            << " | High: " << GREEN << highScore << RESET << "\n";
        out << "Controls: W/A/S/D or ARROW KEYS | Q = Quit | P = Pause/Resume\n";

        // Build the board rows
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const string &cell = grid[y][x];
                // Choose colored representation inline
                if (cell == EMOJI_FOOD) out << RED << cell << RESET;
                else if (cell == EMOJI_SNAKE_HEAD) out << GREEN << cell << RESET;
                else if (cell == EMOJI_SNAKE_BODY) out << GREEN << cell << RESET;
                else if (cell == BORDER_CELL) out << YELLOW << cell << RESET;
                else out << cell;
            }
            out << "\n";
        }

        // Print full frame once
        cout << out.str() << flush;
    }
};

class Game {
private:
    Terminal term;
    GameBoard* board;
    vector<Snake*> snakes;
    Food* food;
    vector<int> scores;
    int highScore, previousScore;
    bool gameOver, running, paused;
    int speedMs, appleCount;
    const int speedStep = 8, minSpeedMs = 30;
    int losingPlayer;

    void setupSnakesAndFood() {
        snakes.resize(NUM_PLAYERS);
        for (int i = 0; i < NUM_PLAYERS; ++i) {
            int sy = board->getHeight() / 2;
            int sx;
            Direction dir = RIGHT;
            if (NUM_PLAYERS == 1) {
                sx = board->getWidth() / 2;
            } else {
                sx = (board->getWidth() * (i + 1)) / (NUM_PLAYERS + 1);
                if (i == 1) dir = LEFT;
            }
            snakes[i] = new Snake(sx, sy, dir);
        }
        food = new Food();
        food->spawn(board->getWidth(), board->getHeight(), snakes);
    }

public:
    Game(int boardSize)
        : highScore(0), previousScore(0),
          gameOver(false), running(true), paused(false),
          speedMs(140), appleCount(0), losingPlayer(0) {

        term.hideCursor();
        ScoreData loaded = loadScores();
        previousScore = loaded.previousScore;
        highScore = loaded.highScore;

        board = new GameBoard(boardSize, boardSize);
        scores.assign(NUM_PLAYERS, 0);
        setupSnakesAndFood();
    }

    ~Game() {
        delete board;
        for (auto s : snakes) delete s;
        delete food;
        term.showCursor();
    }

    void handleInput() {
        if (!term.kbhit()) return;
        char k = term.getch();
        if (k == 0) return;

        if (k == 27) {
            if (term.kbhit()) {
                char b1 = term.getch();
                if (b1 == '[' && term.kbhit()) {
                    char b2 = term.getch();
                    if (b2 == 'A') snakes[0]->setDirection(UP);
                    else if (b2 == 'B') snakes[0]->setDirection(DOWN);
                    else if (b2 == 'C') snakes[0]->setDirection(RIGHT);
                    else if (b2 == 'D') snakes[0]->setDirection(LEFT);
                }
            }
        } else {
            k = toupper(k);
            if (k == 'W') {
                if (NUM_PLAYERS == 1) snakes[0]->setDirection(UP);
                else if (NUM_PLAYERS > 1) snakes[1]->setDirection(UP);
            }
            else if (k == 'S') {
                if (NUM_PLAYERS == 1) snakes[0]->setDirection(DOWN);
                else if (NUM_PLAYERS > 1) snakes[1]->setDirection(DOWN);
            }
            else if (k == 'A') {
                if (NUM_PLAYERS == 1) snakes[0]->setDirection(LEFT);
                else if (NUM_PLAYERS > 1) snakes[1]->setDirection(LEFT);
            }
            else if (k == 'D') {
                if (NUM_PLAYERS == 1) snakes[0]->setDirection(RIGHT);
                else if (NUM_PLAYERS > 1) snakes[1]->setDirection(RIGHT);
            }
            else if (k == 'Q') running = false;
            else if (k == 'P') togglePause();
        }
    }

    void togglePause() {
        paused = !paused;
        if (paused) {
            showPauseScreen();
        }
    }

    void showPauseScreen() {
        // 🧹 Clear bottom section and print pause message
        term.moveCursor(1, board->getHeight() + 6);
        cout << "\n" << YELLOW
             << "\t=============================\n"
             << "\t       GAME PAUSED\n"
             << "\t=============================\n"
             << "\t Press P to Resume\n"
             << RESET << flush;

        // Wait until user presses P again
        while (paused && running) {
            if (term.kbhit()) {
                char k = toupper(term.getch());
                if (k == 'P') {
                    paused = false;
                    term.clearScreen();
                    render(); // redraw fresh frame after resume
                    return;
                } else if (k == 'Q') {
                    running = false;
                    return;
                }
            }
            term.sleep(100);
        }
    }

    void update() {
        for (int i = 0; i < NUM_PLAYERS; ++i) {
            snakes[i]->move();
        }

        vector<bool> lost(NUM_PLAYERS, false);
        for (int i = 0; i < NUM_PLAYERS; ++i) {
            Position head = snakes[i]->getHead();
            if (!board->isInsideBoundaries(head)) lost[i] = true;
            if (snakes[i]->checkSelfCollision()) lost[i] = true;
            for (int j = 0; j < NUM_PLAYERS; ++j) {
                if (i == j) continue;
                for (const auto& part : snakes[j]->getBody()) {
                    if (head == part) { lost[i] = true; break; }
                }
            }
        }

        bool anyLost = false;
        for (int i = 0; i < NUM_PLAYERS; ++i) {
            if (lost[i]) anyLost = true;
        }

        if (anyLost) {
            gameOver = true;
            if (NUM_PLAYERS > 1) {
                if (lost[0] && lost[1]) losingPlayer = 3;
                else if (lost[0]) losingPlayer = 1;
                else losingPlayer = 2;
            }
            return;
        }

        bool foodAte = false;
        for (int i = 0; i < NUM_PLAYERS; ++i) {
            Position head = snakes[i]->getHead();
            if (head == food->getPosition()) {
                snakes[i]->grow();
                scores[i]++;
                appleCount++;
                if (scores[i] > highScore) highScore = scores[i];
                foodAte = true;
            }
        }

        if (foodAte) {
            if (appleCount >= 4) {
                speedMs = max(minSpeedMs, speedMs - speedStep);
                appleCount = 0;
            }
            food->spawn(board->getWidth(), board->getHeight(), snakes);
        }
    }

    void render() {
        board->clear();
        Position fpos = food->getPosition();
        board->place(fpos.x, fpos.y, food->getSymbol());
        for (int p = 0; p < NUM_PLAYERS; ++p) {
            const deque<Position>& body = snakes[p]->getBody();
            for (size_t i = 0; i < body.size(); ++i) {
                board->place(body[i].x, body[i].y,
                             (i == 0) ? snakes[p]->getHeadSymbol() : snakes[p]->getBodySymbol());
            }
        }
        board->render(term, scores, highScore, previousScore);
    }

    void showGameOver() {
        int maxScore = 0;
        for (int s : scores) if (s > maxScore) maxScore = s;
        previousScore = maxScore;
        if (maxScore > highScore) highScore = maxScore;
        saveScores({ previousScore, highScore });

        term.clearScreen();
        term.moveCursor(1, 1);

        cout << "\n\n\t ================================\n";
        cout << "\t         GAME OVER!\n";
        cout << "\t ================================\n\n";
        if (NUM_PLAYERS == 1) {
            cout << "\t   Final Score: " << scores[0] << "\n";
        } else {
            if (losingPlayer == 1)
                cout << "\t   Result: PLAYER 1 LOST!\n";
            else if (losingPlayer == 2)
                cout << "\t   Result: PLAYER 2 LOST!\n";
            else
                cout << "\t   Result: BOTH PLAYERS LOST!\n";
            for (int i = 0; i < NUM_PLAYERS; ++i) {
                cout << "\t   Player " << (i + 1) << " Score: " << scores[i] << "\n";
            }
        }
        cout << "\t   High Score: " << highScore << "\n";
        cout << "\t   Previous Score: " << previousScore << "\n";
        cout << "\n\t ================================\n\n";
        cout << "\t Press R to Restart, Q to Quit\n\n" << flush;

        while (true) {
            if (term.kbhit()) {
                char k = toupper(term.getch());
                if (k == 'R') {
                    term.clearScreen();
                    restart();
                    return;
                } else if (k == 'Q') {
                    term.clearScreen();
                    running = false;
                    return;
                }
            }
            term.sleep(50);
        }
    }

    void restart() {
        for (auto s : snakes) delete s;
        snakes.clear();
        delete food;
        fill(scores.begin(), scores.end(), 0);
        losingPlayer = 0;
        gameOver = false;
        paused = false;
        speedMs = 140;
        appleCount = 0;

        setupSnakesAndFood();

        term.clearScreen();
        term.moveCursor(1, 1);
    }

    void run() {
        render();
        while (running) {
            if (gameOver) {
                showGameOver();
            } else if (!paused) {
                handleInput();
                update();
                render();
                term.sleep(speedMs);
            } else {
                // still handle pause input while paused
                handleInput();
                term.sleep(100);
            }
        }
    }
};



// ======================================================
// MAIN
// ======================================================
int main() {
    srand((unsigned)time(0));
    Terminal term;

    int consoleW, consoleH;
    term.getSize(consoleW, consoleH);

    int gameSize = min((consoleW - 2) / 2, consoleH - 6);
    if (gameSize < 10) gameSize = 10;

    term.clearScreen();
    term.hideCursor();

#ifdef _WIN32
    cout << "[Windows mode detected — ASCII graphics]\n\n";
#else
    cout << "[Unix mode detected — Emoji graphics]\n\n";
#endif

    cout << "===================================\n";
    cout << "       WELCOME TO SNAKE GAME\n";
    cout << "===================================\n\n";
    cout << "Saved High Score: " << loadScores().highScore << "\n\n";
    cout << "Instructions:\n";
    if (NUM_PLAYERS == 1) {
        cout << "  - Use W/A/S/D or ARROW KEYS to control the snake\n";
    } else {
        cout << "  - Player 1: ARROW KEYS to move\n";
        cout << "  - Player 2: W/A/S/D to move\n";
    }
    cout << "  - Eat " << EMOJI_FOOD << " to grow and score\n";
    cout << "  - Speed increases after every 4 apples eaten\n";
    cout << "  - Avoid walls and yourself\n";
    cout << "  - Press Q to quit anytime\n\n";
    cout << "  - Press P to Pause/Resume anytime\n\n";
    cout << "Press any key to start...\n";

    while (!term.kbhit()) term.sleep(50);
    term.getch();

    Game game(gameSize);
    game.run();

    term.clearScreen();
    term.showCursor();
    cout << "\nThank you for playing! Your scores are saved to scores.txt\n";
    return 0;
}
