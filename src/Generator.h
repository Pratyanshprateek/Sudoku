#ifndef SUDOKU_GENERATOR_H
#define SUDOKU_GENERATOR_H

#include "Board.h"
#include "Solver.h"

#include <random>

struct PuzzleData {
    Grid puzzle {};
    Grid solution {};
    Difficulty difficulty {Difficulty::Easy};
};

class Generator {
public:
    Generator();

    PuzzleData createPuzzle(Difficulty difficulty);

private:
    bool fillRandom(Grid& grid, int row, int col);
    std::vector<int> shuffledDigits();
    int targetClues(Difficulty difficulty) const;

    Solver solver_ {};
    std::mt19937 rng_ {};
};

#endif
