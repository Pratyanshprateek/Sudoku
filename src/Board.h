#ifndef SUDOKU_BOARD_H
#define SUDOKU_BOARD_H

#include <array>
#include <optional>
#include <set>
#include <string>
#include <vector>

enum class Difficulty {
    Easy,
    Medium,
    Hard
};

using Grid = std::array<std::array<int, 9>, 9>;

struct Position {
    int row {-1};
    int col {-1};

    bool operator<(const Position& other) const {
        if (row != other.row) {
            return row < other.row;
        }
        return col < other.col;
    }

    bool operator==(const Position& other) const {
        return row == other.row && col == other.col;
    }
};

class Board {
public:
    void loadPuzzle(const Grid& puzzle, const Grid& solution, Difficulty difficulty);
    void reset();

    bool setSelected(Position position);
    std::optional<Position> getSelectedCell() const;

    bool placeValue(int value);
    bool clearSelected();
    std::optional<Position> applyHint();
    void revealSolution();
    void checkProgress();

    bool canEdit(Position position) const;
    bool isSolved() const;
    bool isComplete() const;
    bool isInvalidMove(Position position, int value) const;

    int getValue(int row, int col) const;
    bool isOriginalCell(int row, int col) const;
    bool isHighlightedCell(int row, int col) const;
    bool isIncorrectCell(int row, int col) const;

    Difficulty getDifficulty() const;
    std::string getDifficultyName() const;
    std::string getStatusMessage() const;
    std::size_t getHintsUsed() const;

private:
    bool isInside(Position position) const;
    bool hasConflict(Position position, int value, std::set<Position>* conflicts = nullptr) const;
    void clearTransientHighlights();
    void setStatus(const std::string& message);
    void highlightConflicts(Position position, int value);

    Grid initialGrid_ {};
    Grid currentGrid_ {};
    Grid solutionGrid_ {};
    Difficulty difficulty_ {Difficulty::Easy};
    std::optional<Position> selectedCell_ {};
    std::set<Position> conflictCells_ {};
    std::set<Position> incorrectCells_ {};
    std::string statusMessage_ {"Choose a difficulty and start a new puzzle."};
    std::size_t hintsUsed_ {0};
    bool solutionRevealed_ {false};
};

#endif
