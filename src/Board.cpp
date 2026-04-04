#include "Board.h"

#include <sstream>

void Board::loadPuzzle(const Grid& puzzle, const Grid& solution, Difficulty difficulty) {
    initialGrid_ = puzzle;
    currentGrid_ = puzzle;
    solutionGrid_ = solution;
    difficulty_ = difficulty;
    selectedCell_.reset();
    conflictCells_.clear();
    incorrectCells_.clear();
    hintsUsed_ = 0;
    solutionRevealed_ = false;
    setStatus("New puzzle ready. Select a cell and start solving.");
}

void Board::reset() {
    currentGrid_ = initialGrid_;
    selectedCell_.reset();
    conflictCells_.clear();
    incorrectCells_.clear();
    hintsUsed_ = 0;
    solutionRevealed_ = false;
    setStatus("Puzzle reset.");
}

bool Board::setSelected(Position position) {
    if (!isInside(position)) {
        selectedCell_.reset();
        return false;
    }

    selectedCell_ = position;
    clearTransientHighlights();
    return true;
}

std::optional<Position> Board::getSelectedCell() const {
    return selectedCell_;
}

bool Board::placeValue(int value) {
    if (!selectedCell_.has_value()) {
        setStatus("Select a cell first.");
        return false;
    }

    const Position position = *selectedCell_;
    if (!canEdit(position)) {
        setStatus("Original puzzle cells cannot be edited.");
        return false;
    }

    if (value < 1 || value > 9) {
        setStatus("Only values 1 through 9 are allowed.");
        return false;
    }

    clearTransientHighlights();

    if (hasConflict(position, value, &conflictCells_)) {
        conflictCells_.insert(position);
        setStatus("Invalid move: conflicts highlighted in red.");
        return false;
    }

    currentGrid_[position.row][position.col] = value;
    incorrectCells_.erase(position);

    if (value != solutionGrid_[position.row][position.col]) {
        incorrectCells_.insert(position);
        setStatus("Move placed. It fits the grid, but it is not the final solution.");
    } else if (isSolved()) {
        setStatus("Puzzle solved. Beautiful work.");
    } else {
        setStatus("Move placed.");
    }

    return true;
}

bool Board::clearSelected() {
    if (!selectedCell_.has_value()) {
        setStatus("Select a cell first.");
        return false;
    }

    const Position position = *selectedCell_;
    if (!canEdit(position)) {
        setStatus("Original puzzle cells cannot be edited.");
        return false;
    }

    currentGrid_[position.row][position.col] = 0;
    conflictCells_.erase(position);
    incorrectCells_.erase(position);
    setStatus("Cell cleared.");
    return true;
}

std::optional<Position> Board::applyHint() {
    clearTransientHighlights();

    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            if (currentGrid_[row][col] == 0) {
                currentGrid_[row][col] = solutionGrid_[row][col];
                incorrectCells_.erase({row, col});
                ++hintsUsed_;
                selectedCell_ = Position {row, col};

                std::ostringstream message;
                message << "Hint used for row " << (row + 1) << ", column " << (col + 1) << '.';
                setStatus(message.str());
                return Position {row, col};
            }
        }
    }

    setStatus("No empty cells available for a hint.");
    return std::nullopt;
}

void Board::revealSolution() {
    currentGrid_ = solutionGrid_;
    conflictCells_.clear();
    incorrectCells_.clear();
    solutionRevealed_ = true;
    setStatus("Solution revealed.");
}

void Board::checkProgress() {
    clearTransientHighlights();

    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            const int value = currentGrid_[row][col];
            if (value != 0 && value != solutionGrid_[row][col]) {
                incorrectCells_.insert({row, col});
            }
        }
    }

    if (incorrectCells_.empty()) {
        if (isSolved()) {
            setStatus("Everything checks out. Puzzle solved.");
        } else {
            setStatus("No incorrect cells found so far.");
        }
    } else {
        setStatus("Incorrect cells highlighted in red.");
    }
}

bool Board::canEdit(Position position) const {
    return isInside(position) && initialGrid_[position.row][position.col] == 0 && !solutionRevealed_;
}

bool Board::isSolved() const {
    return currentGrid_ == solutionGrid_;
}

bool Board::isComplete() const {
    for (const auto& row : currentGrid_) {
        for (const int value : row) {
            if (value == 0) {
                return false;
            }
        }
    }
    return true;
}

bool Board::isInvalidMove(Position position, int value) const {
    return hasConflict(position, value, nullptr);
}

int Board::getValue(int row, int col) const {
    return currentGrid_[row][col];
}

bool Board::isOriginalCell(int row, int col) const {
    return initialGrid_[row][col] != 0;
}

bool Board::isHighlightedCell(int row, int col) const {
    return conflictCells_.count({row, col}) > 0;
}

bool Board::isIncorrectCell(int row, int col) const {
    return incorrectCells_.count({row, col}) > 0;
}

Difficulty Board::getDifficulty() const {
    return difficulty_;
}

std::string Board::getDifficultyName() const {
    switch (difficulty_) {
        case Difficulty::Easy:
            return "Easy";
        case Difficulty::Medium:
            return "Medium";
        case Difficulty::Hard:
            return "Hard";
    }

    return "Unknown";
}

std::string Board::getStatusMessage() const {
    return statusMessage_;
}

std::size_t Board::getHintsUsed() const {
    return hintsUsed_;
}

bool Board::isInside(Position position) const {
    return position.row >= 0 && position.row < 9 && position.col >= 0 && position.col < 9;
}

bool Board::hasConflict(Position position, int value, std::set<Position>* conflicts) const {
    bool foundConflict = false;

    for (int col = 0; col < 9; ++col) {
        if (col != position.col && currentGrid_[position.row][col] == value) {
            foundConflict = true;
            if (conflicts != nullptr) {
                conflicts->insert({position.row, col});
            }
        }
    }

    for (int row = 0; row < 9; ++row) {
        if (row != position.row && currentGrid_[row][position.col] == value) {
            foundConflict = true;
            if (conflicts != nullptr) {
                conflicts->insert({row, position.col});
            }
        }
    }

    const int boxRow = (position.row / 3) * 3;
    const int boxCol = (position.col / 3) * 3;
    for (int row = boxRow; row < boxRow + 3; ++row) {
        for (int col = boxCol; col < boxCol + 3; ++col) {
            if ((row != position.row || col != position.col) && currentGrid_[row][col] == value) {
                foundConflict = true;
                if (conflicts != nullptr) {
                    conflicts->insert({row, col});
                }
            }
        }
    }

    return foundConflict;
}

void Board::clearTransientHighlights() {
    conflictCells_.clear();
}

void Board::setStatus(const std::string& message) {
    statusMessage_ = message;
}

void Board::highlightConflicts(Position position, int value) {
    conflictCells_.clear();
    hasConflict(position, value, &conflictCells_);
    conflictCells_.insert(position);
}
