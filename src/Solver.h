#ifndef SUDOKU_SOLVER_H
#define SUDOKU_SOLVER_H

#include "Board.h"

class Solver {
public:
    bool solve(Grid& grid) const;
    bool isValidMove(const Grid& grid, int row, int col, int value) const;
    int countSolutions(Grid grid, int limit = 2) const;

private:
    bool solveRecursive(Grid& grid) const;
    void countRecursive(Grid& grid, int& total, int limit) const;
    bool findEmpty(const Grid& grid, int& row, int& col) const;
};

#endif
