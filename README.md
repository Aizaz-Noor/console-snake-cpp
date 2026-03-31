# 🐍 Snake Game — Console Edition

A classic **Snake Game** built in **C++** for the Windows console, developed as a first-semester programming project. The game features smooth console rendering, colorful output, increasing difficulty, and a clean start menu — all without any external game libraries.

---

## 📸 Screenshots

| Gameplay | Game Over |
|----------|-----------|
| ![Gameplay](Screenshot%20(15).png) | ![Game Over](Screenshot%20(16).png) |

---

## 🎮 Features

- **Arrow-key controls** — move the snake in all four directions
- **Wrap-around edges** — the snake teleports to the opposite side instead of dying on wall contact
- **Progressive speed** — the game gets faster as your score increases (every food eaten reduces delay by 5ms, minimum 50ms)
- **Self-collision detection** — game ends if the snake runs into itself
- **Win condition** — reach the maximum snake length (100 segments) to win
- **Pause / Resume** — press `Space` to pause and `Space` again to continue
- **Colored rendering** — blue head, green body, red food
- **Score display** — live score shown at the bottom of the board
- **Start menu** — shown once before the first game

---

## 🕹️ Controls

| Key | Action |
|-----|--------|
| `↑ ↓ ← →` | Move the snake |
| `Space` | Pause / Resume |
| `Esc` | Quit the game |
| `Enter` | Restart after Game Over / Win |

---

## 🛠️ Tech Stack

| Category | Detail |
|----------|--------|
| Language | C++ (C++17) |
| Platform | Windows (Console) |
| Compiler | GCC via MSYS2 MinGW UCRT64 |
| IDE | Visual Studio Code |
| Libraries | `<iostream>`, `<conio.h>`, `<windows.h>`, `<ctime>`, `<algorithm>` |

> ⚠️ **Windows Only** — This project uses `windows.h`, `conio.h`, and `Sleep()` which are Windows-specific APIs. It will not compile on Linux/macOS without porting.

---

## 🚀 How to Build & Run

### Prerequisites
- [MSYS2](https://www.msys2.org/) with MinGW UCRT64 installed
- GCC available at `C:\msys64\ucrt64\bin\gcc.exe`

### Option 1 — Using VS Code
1. Open the project folder in **VS Code**
2. Press `Ctrl+Shift+B` to build the active file using the included `tasks.json`
3. Run the generated `.exe` in a terminal

### Option 2 — Manual Compilation
```bash
gcc -o Snake.exe Snake.cpp
.\Snake.exe
```

### Option 3 — Run the pre-built binary
A pre-built `Snake.exe` is included. Simply double-click it or run it from the terminal:
```bash
.\Snake.exe
```

---

## 📁 Project Structure

```
snake-game/
├── Snake.cpp          # Main game source file
├── fc.cpp             # Alternate / backup version of the game
├── Snake.exe          # Pre-compiled Windows executable
├── .vscode/
│   └── tasks.json     # VS Code build task (GCC via MSYS2)
├── Screenshot (15).png
├── Screenshot (16).png
├── Screenshot (17).png
│   ... (gameplay screenshots)
└── README.md
```

---

## 🧠 How It Works

The game is built around a simple game loop:

```
Loop:
  1. Erase tail
  2. Update snake positions (shift body, move head)
  3. Check self-collision → Game Over
  4. Check food eaten → grow, add score, spawn new food
  5. Draw snake
  6. Handle input
  7. Sleep(speed)
```

Key implementation details:
- **Snake body** is stored as two parallel arrays `snakeX[]` and `snakeY[]`
- **Movement** shifts every segment to the previous segment's position, then moves the head
- **Food spawning** retries up to 500 times to find a cell not occupied by the snake
- **Speed scaling** reduces sleep delay by 5ms per food eaten, capped at 50ms minimum

---

## 👤 Author

**[Your Name]** — First Semester CS Project  
Feel free to fork, star ⭐, or open issues!

---

## 📜 License

This project is open source and available under the [MIT License](LICENSE).
