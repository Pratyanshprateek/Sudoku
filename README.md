# Sudoku GUI (C++ & SFML)

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![SFML](https://img.shields.io/badge/SFML-3.0%2B-brightgreen.svg)
![Status](https://img.shields.io/badge/status-GUI%20ready-success.svg)

A complete desktop Sudoku experience built with modern C++ and SFML. This project combines a production-quality puzzle engine with a polished dark-themed interface, clickable cells, keyboard controls, difficulty switching, timer tracking, validation feedback, and unique-solution puzzle generation.

The project is structured to work across macOS, Linux, and Windows. Assets are loaded relative to the executable location, so the bundled font resolves correctly whether you run the app from the project root, the build folder, or by double-clicking the executable.

## 🎬 Demo

[![Watch Demo](https://img.shields.io/badge/▶%20Watch%20Demo-blue?style=for-the-badge)](https://pratyanshprateek.github.io/uploads/Sudoku-demo.mov)

## Features

- Interactive 9x9 Sudoku board rendered with SFML
- Mouse-based cell selection
- Keyboard input for `1-9`, `Backspace`, and `Delete`
- Difficulty buttons for Easy, Medium, and Hard
- Random puzzle generator with unique-solution validation
- Built-in backtracking solver
- `New Game`, `Reset`, `Check`, `Hint`, and `Solve` buttons
- Timer display and hint tracking
- Dark modern UI with:
  - Thick 3x3 subgrid borders
  - Thin cell borders
  - Selected cell highlight
  - Row and column highlight
  - Red invalid/incorrect cell feedback
  - Distinct colors for original clues and player-entered values

## Tech Stack

- C++17
- SFML
- CMake
- Recursive backtracking for solving and puzzle generation

## Project Structure

```text
Sudoku/
├── assets/
│   └── fonts/
│       └── Geneva.ttf
├── CMakeLists.txt
├── README.md
└── src/
    ├── Board.cpp
    ├── Board.h
    ├── Game.cpp
    ├── Game.h
    ├── Generator.cpp
    ├── Generator.h
    ├── Solver.cpp
    ├── Solver.h
    └── main.cpp
```

## Controls

### Mouse

- Left click a cell to select it
- Left click any button to trigger its action

### Keyboard

- `1-9`: place a number in the selected cell
- `Backspace` / `Delete`: clear the selected editable cell
- Arrow keys: move the selected cell
- `N`: start a new puzzle using the current difficulty
- `R`: reset the current puzzle
- `H`: use a hint

## How It Works

### Solver

The solver uses recursive backtracking:

1. Find the next empty cell.
2. Try values `1` through `9`.
3. Keep only values that satisfy row, column, and 3x3 box constraints.
4. Recurse until the board is solved or backtrack on failure.

### Generator

The generator first creates a complete valid solution grid using randomized backtracking. It then removes numbers one by one while checking the number of possible solutions after each removal. A removal is kept only when the puzzle still has exactly one solution.

## Platform Support

- macOS: supported and verified locally with Homebrew SFML 3.0.2
- Linux: supported through executable-relative asset loading and standard SFML package installation
- Windows: supported through executable-relative asset loading and CMake-based SFML integration

## Installing SFML

### macOS with Homebrew

```bash
brew install sfml
```

### Ubuntu / Debian

```bash
sudo apt update
sudo apt install libsfml-dev
```

### Windows

Install SFML from the official distribution or a package manager such as `vcpkg`, then make sure CMake can locate the SFML package.

Example with `vcpkg`:

```powershell
vcpkg install sfml
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=[path-to-vcpkg]/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
.\build\Release\sudoku_gui.exe
```

## Build Instructions

```bash
cd Sudoku
cmake -S . -B build
cmake --build build
./build/sudoku_gui
```

The CMake build copies the `assets/` directory into the build output automatically, so the bundled font is available at runtime.

### Windows Run Command

If you build with Visual Studio or a multi-config generator, run:

```powershell
.\build\Release\sudoku_gui.exe
```

### Linux Run Command

```bash
./build/sudoku_gui
```

## Notes

- The app highlights conflicting or incorrect cells in red.
- Original clue values are rendered in a neutral bright tone.
- User-entered values are rendered in green.
- The `Solve` button reveals the full solution immediately.
- Asset loading is executable-relative, which makes the packaged `assets/fonts/Geneva.ttf` discoverable on macOS, Linux, and Windows.

## Author

**Pratyansh Prateek**  
[LinkedIn](https://www.linkedin.com/in/pratyansh-prateek/)
