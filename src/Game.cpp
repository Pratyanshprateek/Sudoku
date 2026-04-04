#include "Game.h"

#include <SFML/System.hpp>

#include <filesystem>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#endif

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#endif

namespace {
constexpr float kHeaderHeight = 110.0f;
constexpr float kFooterHeight = 70.0f;

sf::Color withAlpha(sf::Color color, std::uint8_t alpha) {
    color.a = alpha;
    return color;
}

std::filesystem::path executableDirectory() {
#if defined(_WIN32)
    std::wstring buffer(MAX_PATH, L'\0');
    const DWORD length = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
    if (length > 0) {
        buffer.resize(length);
        return std::filesystem::path(buffer).parent_path();
    }
#elif defined(__linux__)
    std::vector<char> buffer(4096, '\0');
    const ssize_t length = readlink("/proc/self/exe", buffer.data(), buffer.size() - 1);
    if (length > 0) {
        buffer[static_cast<std::size_t>(length)] = '\0';
        return std::filesystem::path(buffer.data()).parent_path();
    }
#elif defined(__APPLE__)
    uint32_t size = 0;
    _NSGetExecutablePath(nullptr, &size);
    std::string buffer(size, '\0');
    if (_NSGetExecutablePath(buffer.data(), &size) == 0) {
        return std::filesystem::path(buffer.c_str()).parent_path();
    }
#endif
    return std::filesystem::current_path();
}
}

bool Game::Button::contains(sf::Vector2f point) const {
    return shape.getGlobalBounds().contains(point);
}

Game::Game()
    : window_(sf::VideoMode({900u, 760u}), "Sudoku GUI"),
      titleText_(font_),
      subtitleText_(font_),
      timerText_(font_),
      statusText_(font_) {
    window_.setFramerateLimit(60);
    loadAssets();
    setupUi();
    startNewGame(currentDifficulty_);
}

void Game::run() {
    while (window_.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void Game::loadAssets() {
    const std::string fontPath = assetPath("assets/fonts/Geneva.ttf");
    if (!font_.openFromFile(fontPath)) {
        throw std::runtime_error("Failed to load font from " + fontPath);
    }
}

void Game::setupUi() {
    titleText_.setFont(font_);
    subtitleText_.setFont(font_);
    timerText_.setFont(font_);
    statusText_.setFont(font_);

    titleText_.setPosition({42.0f, 18.0f});
    subtitleText_.setPosition({44.0f, 60.0f});
    timerText_.setPosition({680.0f, 28.0f});
    statusText_.setPosition({42.0f, 700.0f});

    updateText(titleText_, "Sudoku", 34, originalNumberColor_);
    updateText(subtitleText_, "Modern SFML desktop edition", 16, withAlpha(originalNumberColor_, 190));
    updateText(timerText_, "00:00", 28, userNumberColor_);
    updateText(statusText_, "", 18, withAlpha(originalNumberColor_, 200));

    const std::vector<std::pair<std::string, std::string>> buttonSpecs = {
        {"easy", "Easy"},
        {"medium", "Medium"},
        {"hard", "Hard"},
        {"new", "New Game"},
        {"reset", "Reset"},
        {"check", "Check"},
        {"hint", "Hint"},
        {"solve", "Solve"}
    };

    buttons_.clear();
    buttons_.reserve(buttonSpecs.size());

    float x = 42.0f;
    const float y = 104.0f;
    for (const auto& [id, label] : buttonSpecs) {
        Button button(font_);
        button.id = id;
        button.shape.setPosition({x, y});
        button.shape.setSize({(id == "new" || id == "reset" || id == "check" || id == "solve") ? 104.0f : 86.0f, 36.0f});
        button.shape.setFillColor(panelAccent_);
        button.shape.setOutlineThickness(1.0f);
        button.shape.setOutlineColor(withAlpha(originalNumberColor_, 35));

        button.label.setFont(font_);
        updateText(button.label, label, 16, buttonTextColor_, true);
        button.label.setPosition({
            button.shape.getPosition().x + button.shape.getSize().x / 2.0f,
            button.shape.getPosition().y + 18.0f
        });

        buttons_.push_back(button);
        x += button.shape.getSize().x + 12.0f;
    }

    updateButtonStyles();
}

void Game::startNewGame(Difficulty difficulty) {
    currentDifficulty_ = difficulty;
    const PuzzleData puzzle = generator_.createPuzzle(difficulty);
    board_.loadPuzzle(puzzle.puzzle, puzzle.solution, difficulty);
    gameStart_ = std::chrono::steady_clock::now();
    solvedByReveal_ = false;
    updateButtonStyles();
}

void Game::resetCurrentGame() {
    board_.reset();
    gameStart_ = std::chrono::steady_clock::now();
    solvedByReveal_ = false;
}

void Game::processEvents() {
    while (const std::optional event = window_.pollEvent()) {
        handleEvent(*event);
    }
}

void Game::handleEvent(const sf::Event& event) {
    if (event.is<sf::Event::Closed>()) {
        window_.close();
        return;
    }

    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mousePressed->button == sf::Mouse::Button::Left) {
            handleMousePressed(window_.mapPixelToCoords(mousePressed->position));
        }
        return;
    }

    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        handleKeyPressed(*keyPressed);
        return;
    }

    if (const auto* textEntered = event.getIf<sf::Event::TextEntered>()) {
        handleTextEntered(textEntered->unicode);
    }
}

void Game::handleMousePressed(sf::Vector2f position) {
    for (const Button& button : buttons_) {
        if (!button.contains(position)) {
            continue;
        }

        if (button.id == "easy") {
            startNewGame(Difficulty::Easy);
        } else if (button.id == "medium") {
            startNewGame(Difficulty::Medium);
        } else if (button.id == "hard") {
            startNewGame(Difficulty::Hard);
        } else if (button.id == "new") {
            startNewGame(currentDifficulty_);
        } else if (button.id == "reset") {
            resetCurrentGame();
        } else if (button.id == "check") {
            board_.checkProgress();
        } else if (button.id == "hint") {
            board_.applyHint();
        } else if (button.id == "solve") {
            board_.revealSolution();
            solvedByReveal_ = true;
        }

        updateButtonStyles();
        return;
    }

    selectCellFromMouse(position);
}

void Game::handleKeyPressed(const sf::Event::KeyPressed& key) {
    if (key.code == sf::Keyboard::Key::Backspace || key.code == sf::Keyboard::Key::Delete) {
        board_.clearSelected();
        return;
    }

    if (key.code == sf::Keyboard::Key::H) {
        board_.applyHint();
        return;
    }

    if (key.code == sf::Keyboard::Key::R) {
        resetCurrentGame();
        return;
    }

    if (key.code == sf::Keyboard::Key::N) {
        startNewGame(currentDifficulty_);
        return;
    }

    const auto selected = board_.getSelectedCell();
    if (!selected.has_value()) {
        return;
    }

    Position position = *selected;
    if (key.code == sf::Keyboard::Key::Left && position.col > 0) {
        board_.setSelected({position.row, position.col - 1});
    } else if (key.code == sf::Keyboard::Key::Right && position.col < 8) {
        board_.setSelected({position.row, position.col + 1});
    } else if (key.code == sf::Keyboard::Key::Up && position.row > 0) {
        board_.setSelected({position.row - 1, position.col});
    } else if (key.code == sf::Keyboard::Key::Down && position.row < 8) {
        board_.setSelected({position.row + 1, position.col});
    }
}

void Game::handleTextEntered(std::uint32_t unicode) {
    if (unicode >= '1' && unicode <= '9') {
        board_.placeValue(static_cast<int>(unicode - '0'));
        return;
    }

    if (unicode == '0') {
        board_.clearSelected();
    }
}

void Game::update() {
    updateText(timerText_, formatTime(), 28, userNumberColor_);
    updateText(statusText_, board_.getStatusMessage(), 18, withAlpha(originalNumberColor_, 205));

    if (board_.isSolved() && !solvedByReveal_) {
        updateText(statusText_, "Puzzle solved in " + formatTime() + ". Excellent work.", 18, userNumberColor_);
    }
}

void Game::render() {
    window_.clear(backgroundColor_);
    drawHeader();
    drawBoard();
    drawButtons();
    drawFooter();
    window_.display();
}

void Game::drawHeader() {
    sf::RectangleShape header({windowWidth_ - 40.0f, kHeaderHeight});
    header.setPosition({20.0f, 16.0f});
    header.setFillColor(panelColor_);
    header.setOutlineThickness(1.0f);
    header.setOutlineColor(withAlpha(originalNumberColor_, 30));
    window_.draw(header);

    window_.draw(titleText_);
    window_.draw(subtitleText_);
    window_.draw(timerText_);

    sf::Text meta(font_);
    meta.setPosition({678.0f, 62.0f});
    updateText(meta, "Difficulty: " + board_.getDifficultyName() + "   Hints: " + std::to_string(board_.getHintsUsed()), 16,
        withAlpha(originalNumberColor_, 180));
    window_.draw(meta);
}

void Game::drawBoard() {
    sf::RectangleShape background({cellSize_ * 9.0f, cellSize_ * 9.0f});
    background.setPosition({boardLeft_, boardTop_});
    background.setFillColor(boardColor_);
    background.setOutlineThickness(2.0f);
    background.setOutlineColor(withAlpha(originalNumberColor_, 50));
    window_.draw(background);

    const auto selected = board_.getSelectedCell();

    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            sf::RectangleShape cell({cellSize_, cellSize_});
            cell.setPosition({boardLeft_ + col * cellSize_, boardTop_ + row * cellSize_});
            cell.setFillColor(sf::Color::Transparent);

            if (selected.has_value() && (selected->row == row || selected->col == col)) {
                cell.setFillColor(relatedColor_);
            }

            if (board_.isHighlightedCell(row, col) || board_.isIncorrectCell(row, col)) {
                cell.setFillColor(withAlpha(invalidNumberColor_, 58));
            }

            if (selected.has_value() && selected->row == row && selected->col == col) {
                cell.setFillColor(selectedColor_);
            }

            window_.draw(cell);

            const int value = board_.getValue(row, col);
            if (value != 0) {
                sf::Text text(font_);

                sf::Color color = board_.isOriginalCell(row, col) ? originalNumberColor_ : userNumberColor_;
                if (board_.isHighlightedCell(row, col) || board_.isIncorrectCell(row, col)) {
                    color = invalidNumberColor_;
                }

                updateText(text, std::to_string(value), 30, color, true);
                text.setPosition({
                    boardLeft_ + col * cellSize_ + cellSize_ / 2.0f,
                    boardTop_ + row * cellSize_ + cellSize_ / 2.0f - 2.0f
                });
                window_.draw(text);
            }
        }
    }

    for (int i = 0; i <= 9; ++i) {
        sf::RectangleShape vertical({(i % 3 == 0) ? 3.0f : 1.0f, cellSize_ * 9.0f});
        vertical.setPosition({boardLeft_ + i * cellSize_ - ((i % 3 == 0) ? 1.5f : 0.5f), boardTop_});
        vertical.setFillColor(withAlpha(originalNumberColor_, i % 3 == 0 ? 190 : 70));
        window_.draw(vertical);

        sf::RectangleShape horizontal({cellSize_ * 9.0f, (i % 3 == 0) ? 3.0f : 1.0f});
        horizontal.setPosition({boardLeft_, boardTop_ + i * cellSize_ - ((i % 3 == 0) ? 1.5f : 0.5f)});
        horizontal.setFillColor(withAlpha(originalNumberColor_, i % 3 == 0 ? 190 : 70));
        window_.draw(horizontal);
    }
}

void Game::drawButtons() {
    for (const Button& button : buttons_) {
        window_.draw(button.shape);
        window_.draw(button.label);
    }
}

void Game::drawFooter() {
    sf::RectangleShape footer({windowWidth_ - 40.0f, kFooterHeight});
    footer.setPosition({20.0f, windowHeight_ - 52.0f - kFooterHeight});
    footer.setFillColor(panelColor_);
    footer.setOutlineThickness(1.0f);
    footer.setOutlineColor(withAlpha(originalNumberColor_, 30));
    window_.draw(footer);

    window_.draw(statusText_);

    sf::Text controls(font_);
    controls.setPosition({42.0f, 726.0f});
    updateText(controls, "Mouse: select cell or button   Keyboard: 1-9 place value, Backspace/Delete clear, N new, R reset, H hint",
        14, withAlpha(originalNumberColor_, 150));
    window_.draw(controls);
}

void Game::updateText(sf::Text& text, const std::string& content, unsigned int size, sf::Color color, bool centered) {
    text.setString(content);
    text.setCharacterSize(size);
    text.setFillColor(color);

    const sf::FloatRect bounds = text.getLocalBounds();
    if (centered) {
        text.setOrigin({bounds.position.x + bounds.size.x / 2.0f, bounds.position.y + bounds.size.y / 2.0f});
    } else {
        text.setOrigin({0.0f, 0.0f});
    }
}

void Game::updateButtonStyles() {
    for (Button& button : buttons_) {
        const bool activeDifficulty =
            (button.id == "easy" && currentDifficulty_ == Difficulty::Easy) ||
            (button.id == "medium" && currentDifficulty_ == Difficulty::Medium) ||
            (button.id == "hard" && currentDifficulty_ == Difficulty::Hard);

        button.shape.setFillColor(activeDifficulty ? highlightButtonColor_ : panelAccent_);
        button.shape.setOutlineColor(activeDifficulty ? withAlpha(userNumberColor_, 200) : withAlpha(originalNumberColor_, 35));
    }
}

void Game::selectCellFromMouse(sf::Vector2f position) {
    if (position.x < boardLeft_ || position.x > boardLeft_ + cellSize_ * 9.0f ||
        position.y < boardTop_ || position.y > boardTop_ + cellSize_ * 9.0f) {
        board_.setSelected({-1, -1});
        return;
    }

    const int col = static_cast<int>((position.x - boardLeft_) / cellSize_);
    const int row = static_cast<int>((position.y - boardTop_) / cellSize_);
    board_.setSelected({row, col});
}

std::string Game::formatTime() const {
    const auto elapsed = std::chrono::steady_clock::now() - gameStart_;
    const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
    const long minutes = seconds / 60;
    const long remainder = seconds % 60;

    std::ostringstream stream;
    stream << std::setw(2) << std::setfill('0') << minutes
           << ':'
           << std::setw(2) << std::setfill('0') << remainder;
    return stream.str();
}

std::string Game::assetPath(const std::string& relativePath) const {
    const std::vector<std::filesystem::path> candidates = {
        executableDirectory() / relativePath,
        executableDirectory().parent_path() / relativePath,
        std::filesystem::current_path() / relativePath,
        std::filesystem::current_path() / ".." / relativePath,
        std::filesystem::current_path() / ".." / ".." / relativePath
    };

    for (const auto& path : candidates) {
        if (std::filesystem::exists(path)) {
            return path.string();
        }
    }

    return relativePath;
}
