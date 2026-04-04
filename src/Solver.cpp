#include "Solver.h"

bool Solver::solve(Grid& grid) const {
    return solveRecursive(grid);
}

bool Solver::isValidMove(const Grid& grid, int row, int col, int value) const {
    for (int currentCol = 0; currentCol < 9; ++currentCol) {
        if (currentCol != col && grid[row][currentCol] == value) {
            return false;
        }
    }

    for (int currentRow = 0; currentRow < 9; ++currentRow) {
        if (currentRow != row && grid[currentRow][col] == value) {
            return false;
        }
    }

    const int boxRow = (row / 3) * 3;
    const int boxCol = (col / 3) * 3;
    for (int currentRow = boxRow; currentRow < boxRow + 3; ++currentRow) {
        for (int currentCol = boxCol; currentCol < boxCol + 3; ++currentCol) {
            if ((currentRow != row || currentCol != col) && grid[currentRow][currentCol] == value) {
                return false;
            }
        }
    }

    return true;
}

int Solver::countSolutions(Grid grid, int limit) const {
    int total = 0;
    countRecursive(grid, total, limit);
    return total;
}

bool Solver::solveRecursive(Grid& grid) const {
    int row = 0;
    int col = 0;
    if (!findEmpty(grid, row, col)) {
        return true;
    }

    for (int value = 1; value <= 9; ++value) {
        if (!isValidMove(grid, row, col, value)) {
            continue;
        }

        grid[row][col] = value;
        if (solveRecursive(grid)) {
            return true;
        }
        grid[row][col] = 0;
    }

    return false;
}

void Solver::countRecursive(Grid& grid, int& total, int limit) const {
    if (total >= limit) {
        return;
    }

    int row = 0;
    int col = 0;
    if (!findEmpty(grid, row, col)) {
        ++total;
        return;
    }

    for (int value = 1; value <= 9; ++value) {
        if (!isValidMove(grid, row, col, value)) {
            continue;
        }

        grid[row][col] = value;
        countRecursive(grid, total, limit);
        grid[row][col] = 0;

        if (total >= limit) {
            return;
        }
    }
}

bool Solver::findEmpty(const Grid& grid, int& row, int& col) const {
    for (row = 0; row < 9; ++row) {
        for (col = 0; col < 9; ++col) {
            if (grid[row][col] == 0) {
                return true;
            }
        }
    }

    return false;
}
