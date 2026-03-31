#include <iostream>
#include <conio.h>       // For getch() and kbhit()
#define NOMINMAX         // Prevent Windows.h from defining max/min macros
#include <windows.h>     // For gotoxy() and sleep
#include <ctime>       
#include <algorithm>     // For std::max

using namespace std;

// Constants for game dimensions and snake settings
const int WIDTH = 75;               // Width of the game board
const int HEIGHT = 17;              // Height of the game board
const int MAX_SNAKE_SIZE = 100;     // Max number of body parts

// Playfield drawing offsets (to avoid magic numbers)
const int BORDER_LEFT = 2;
const int BORDER_TOP = 5;
const int PLAY_X_MIN = 4;           // First playable x (aligns with even columns)
const int PLAY_Y_MIN = 6;           // First playable y (below title and border)

// Keyboard codes (for readability)
const int KEY_ESC = 27;
const int KEY_SPACE = ' ';
const int KEY_ENTER = 13;
const int KEY_ARROW_PREFIX = -32;   // First code returned before arrow key code
const int KEY_UP = 72;
const int KEY_DOWN = 80;
const int KEY_LEFT = 75;
const int KEY_RIGHT = 77;

// Enum for direction handling
enum Direction { LEFT = 1, RIGHT, UP, DOWN };

// Global variables
int snakeX[MAX_SNAKE_SIZE], snakeY[MAX_SNAKE_SIZE]; // Snake body coordinates
int snakeSize ;                                  // Initial snake size
int foodX, foodY;                                   // Food coordinates
int score = 0;                                      // Score tracker
Direction dir = RIGHT;                              // Initial direction
bool gameRunning = true;                            // Controls game loop
bool restartGame = true;                            // Restart flag
int speed = 200;                                    // Initial game speed (ms)
WORD defaultConsoleAttributes = 7;                  // To restore colors on exit
bool hasShownMenu = false;                          // Show start menu once

// Moves the cursor to the given x, y in console window
void gotoxy(int x, int y) {
    COORD coord = { SHORT(x), SHORT(y) };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Hides the blinking text cursor in the console
void hideCursor() {
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursor = { 1, false };
    SetConsoleCursorInfo(hout, &cursor);
}

// Sets the console text attribute (color)
void setColor(WORD attr) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attr);
}

// Simple helper to print centered text at a given row
void printCentered(int row, const char* text) {
    int len = (int)strlen(text);
    int col = max(0, (WIDTH + BORDER_LEFT + 2 - len) / 2);
    gotoxy(col, row);
    cout << text;
}

// Draws the title and the rectangular game boundary using # characters
void drawBorders() {
    system("cls");  // Clear screen

    // Display the title "Snake Game"
    gotoxy(30, 1); cout << "=================";
    gotoxy(30, 2); cout << "   SNAKE GAME   ";
    gotoxy(30, 3); cout << "=================";
    gotoxy(30, 4); cout << "                     Use Arrow Keys to control! ";
    
    setColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    for (int x = BORDER_LEFT; x <= WIDTH + BORDER_LEFT; x++) {
        gotoxy(x, BORDER_TOP); cout << "-";              // Top border
        gotoxy(x, HEIGHT + BORDER_TOP + 1); cout << "-"; // Bottom border
    }
    for (int y = BORDER_TOP; y <= HEIGHT + BORDER_TOP + 1; y++) { 
        gotoxy(BORDER_LEFT, y); cout << "|";              // Left border
        gotoxy(WIDTH + BORDER_LEFT, y); cout << "|";       // Right border
    }
    setColor(defaultConsoleAttributes);
}

// Randomly places the food on screen, avoiding snake body
void spawnFood() {
    // Guard against rare case where there is no free cell
    int attempts = 0;
    const int maxAttempts = 500;
    while (true) {
        // Random food position in even columns for spacing
        foodX = rand() % (WIDTH / 2) * 2 + PLAY_X_MIN;
        foodY = rand() % HEIGHT + PLAY_Y_MIN; // Adjusted for new starting Y

        bool onSnake = false;
        for (int i = 0; i < snakeSize; i++) {
            if (snakeX[i] == foodX && snakeY[i] == foodY) {
                onSnake = true; break;
            }
        }
        if (!onSnake) break;  // Valid food location
        attempts++;
        if (attempts > maxAttempts) {
            // Consider this a win state (no space left)
            gotoxy(25, HEIGHT + BORDER_TOP + 3);
            cout << "You Win! No more space left. Press Enter to restart.";
            gameRunning = false;
            restartGame = true;
            return;
        }
    }

    gotoxy(foodX, foodY);
    setColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
    cout << "#";  // Print food symbol
    setColor(defaultConsoleAttributes);
}

// Draws the snake on screen
void drawSnake() {
    for (int i = 0; i < snakeSize; i++) {
        gotoxy(snakeX[i], snakeY[i]);
        if (i == 0) {
            setColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
            cout << "O";  // Head
        } else {
            setColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            cout << "*";  // Body
        }
        setColor(defaultConsoleAttributes);
    }
}

// Erases the last tail block from previous move
void eraseTail() {
    gotoxy(snakeX[snakeSize - 1], snakeY[snakeSize - 1]);
    cout << " ";
}

// Updates the snake's body by moving every segment
void updateSnakePosition() {
    for (int i = snakeSize - 1; i > 0; i--) {
        snakeX[i] = snakeX[i - 1];
        snakeY[i] = snakeY[i - 1];
    }

    // Move the head based on direction
    switch (dir) {
        case RIGHT: snakeX[0] += 2; break;
        case LEFT:  snakeX[0] -= 2; break;
        case UP:    snakeY[0] -= 1; break;
        case DOWN:  snakeY[0] += 1; break;
    }

    // Wrap-around edges (teleport to other side)
    if (snakeX[0] > WIDTH + 1) snakeX[0] = PLAY_X_MIN;
    if (snakeX[0] < PLAY_X_MIN)         snakeX[0] = WIDTH;
    if (snakeY[0] > HEIGHT + 5) snakeY[0] = PLAY_Y_MIN; // Adjusted for new starting Y
    if (snakeY[0] < PLAY_Y_MIN)         snakeY[0] = HEIGHT + 5;
}

// Checks if the snake's head collided with its body
bool checkSelfCollision() {
    for (int i = 1; i < snakeSize; i++) {
        if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i])
            return true;
    }
    return false;
}

// Displays the score in the top right corner
void printScore() {
    gotoxy(60, HEIGHT + 8);
    cout << "Score: " << score * 10;
}

// Called when the game ends due to self-collision
void gameOver() {
    gotoxy(25, HEIGHT + 10);
    cout << "Game Over! Press Enter to restart or Esc to quit.";
    gameRunning = false;
    restartGame = true;
}

// Handles keyboard input: arrow keys, pause, exit
void handleInput() {
    if (_kbhit()) {
        char ch = _getch();

        if (ch == KEY_SPACE) {  // Pause 
            gotoxy(30, HEIGHT + 10); cout << "Paused. Press space to continue.";
            while (_getch() != ' ');
            gotoxy(30, HEIGHT + 10); cout << "                                ";
        } else if (ch == KEY_ESC) {  // ESC to exit
            gameRunning = false;
            exit(0);
        } else if (ch == KEY_ARROW_PREFIX) {  // Arrow keys
            ch = _getch();  // Get arrow code
            if (ch == KEY_UP && dir != DOWN) dir = UP;
            if (ch == KEY_DOWN && dir != UP) dir = DOWN;
            if (ch == KEY_LEFT && dir != RIGHT) dir = LEFT;
            if (ch == KEY_RIGHT && dir != LEFT) dir = RIGHT;
        }
    }
}

// Initializes the game variables and screen
void initGame() {
    srand((unsigned)time(0));  // Seed RNG
    drawBorders();             // Draw the play area

    // Reset variables
    score = 0;
    speed = 200;
    snakeSize = 2;
    dir = RIGHT;
    gameRunning = true;
    restartGame = false;

    // Place snake in initial position
    for (int i = 0; i < snakeSize; i++) {
        snakeX[i] = 20 - i * 2;
        snakeY[i] = 10; // Adjusted for new starting Y
    }

    drawSnake();      // Draw initial snake
    spawnFood();      // Place first food
    printScore();     // Show initial score
}

// Simple start menu shown once
void showStartMenu() {
    system("cls");
    setColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    printCentered(3, "=================");
    printCentered(4, "   SNAKE GAME   ");
    printCentered(5, "=================");
    setColor(defaultConsoleAttributes);
    printCentered(7, "Controls: Arrow Keys to move, Space to pause, Esc to quit");
    printCentered(9, "Press Enter to start...");
    while (_getch() != KEY_ENTER);
}

// Main game loop
int main() {
    hideCursor();  

    // Capture default console attributes to restore later
    CONSOLE_SCREEN_BUFFER_INFO csbi; 
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        defaultConsoleAttributes = csbi.wAttributes;
    }

    if (!hasShownMenu) {
        showStartMenu();
        hasShownMenu = true;
    }

    while (true) {
        if (restartGame) {
            initGame();  // Reset game
        }

        if (!gameRunning) {
            while (_getch() != 13);  // Wait for Enter to restart
            continue;
        }

        eraseTail();            // Remove last tail before moving
        updateSnakePosition();  // Move snake

        if (checkSelfCollision()) {
            gameOver();         // End game on collision
            continue;
        }

        // If food is eaten
        if (snakeX[0] == foodX && snakeY[0] == foodY) {
            if (snakeSize < MAX_SNAKE_SIZE) {
                snakeSize++;               // Grow snake
            }
            if (snakeSize >= MAX_SNAKE_SIZE) {
                gotoxy(25, HEIGHT + BORDER_TOP + 3);
                cout << "You Win! Maximum length reached. Press Enter to restart.";
                gameRunning = false;
                restartGame = true;
            }
            score++;                   // Add score
            speed = std::max(speed - 5, 50); // Speed up (minimum delay 50ms)
            spawnFood();              // Spawn new food
            printScore();             // Update score display
        }

        drawSnake();      // Draw snake after move
        handleInput();    // Check for user input
        Sleep(speed);     // Wait to control game speed
    }

    return 0;
}
