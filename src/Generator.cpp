#include "Generator.h"

#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <vector>

Generator::Generator()
    : rng_(std::random_device {}()) {}

PuzzleData Generator::createPuzzle(Difficulty difficulty) {
    Grid solution {};
    if (!fillRandom(solution, 0, 0)) {
        throw std::runtime_error("Unable to generate a complete Sudoku solution.");
    }

    Grid puzzle = solution;
    std::vector<Position> cells;
    cells.reserve(81);
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            cells.push_back({row, col});
        }
    }

    std::shuffle(cells.begin(), cells.end(), rng_);
    int clues = 81;
    const int target = targetClues(difficulty);

    for (const Position& position : cells) {
        if (clues <= target) {
            break;
        }

        const int previous = puzzle[position.row][position.col];
        puzzle[position.row][position.col] = 0;

        if (solver_.countSolutions(puzzle, 2) != 1) {
            puzzle[position.row][position.col] = previous;
        } else {
            --clues;
        }
    }

    return PuzzleData {puzzle, solution, difficulty};
}

bool Generator::fillRandom(Grid& grid, int row, int col) {
    if (row == 9) {
        return true;
    }

    const int nextRow = (col == 8) ? row + 1 : row;
    const int nextCol = (col + 1) % 9;

    for (const int value : shuffledDigits()) {
        if (!solver_.isValidMove(grid, row, col, value)) {
            continue;
        }

        grid[row][col] = value;
        if (fillRandom(grid, nextRow, nextCol)) {
            return true;
        }
        grid[row][col] = 0;
    }

    return false;
}

std::vector<int> Generator::shuffledDigits() {
    std::vector<int> digits(9);
    std::iota(digits.begin(), digits.end(), 1);
    std::shuffle(digits.begin(), digits.end(), rng_);
    return digits;
}

int Generator::targetClues(Difficulty difficulty) const {
    switch (difficulty) {
        case Difficulty::Easy:
            return 40;
        case Difficulty::Medium:
            return 32;
        case Difficulty::Hard:
            return 26;
    }

    return 32;
}
