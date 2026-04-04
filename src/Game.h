#ifndef SUDOKU_GAME_H
#define SUDOKU_GAME_H

#include "Board.h"
#include "Generator.h"

#include <SFML/Graphics.hpp>

#include <chrono>
#include <string>
#include <vector>

class Game {
public:
    Game();
    void run();

private:
    struct Button {
        explicit Button(const sf::Font& font)
            : label(font) {}

        sf::RectangleShape shape;
        sf::Text label;
        std::string id;

        bool contains(sf::Vector2f point) const;
    };

    void loadAssets();
    void setupUi();
    void startNewGame(Difficulty difficulty);
    void resetCurrentGame();

    void processEvents();
    void handleEvent(const sf::Event& event);
    void handleMousePressed(sf::Vector2f position);
    void handleKeyPressed(const sf::Event::KeyPressed& key);
    void handleTextEntered(std::uint32_t unicode);

    void update();
    void render();
    void drawHeader();
    void drawBoard();
    void drawButtons();
    void drawFooter();

    void updateText(sf::Text& text, const std::string& content, unsigned int size, sf::Color color, bool centered = false);
    void updateButtonStyles();
    void selectCellFromMouse(sf::Vector2f position);
    std::string formatTime() const;
    std::string assetPath(const std::string& relativePath) const;

    sf::RenderWindow window_;
    sf::Font font_;

    Board board_ {};
    Generator generator_ {};
    Difficulty currentDifficulty_ {Difficulty::Easy};
    bool solvedByReveal_ {false};

    std::chrono::steady_clock::time_point gameStart_ {};

    sf::Text titleText_;
    sf::Text subtitleText_;
    sf::Text timerText_;
    sf::Text statusText_;
    sf::Text authorText_;
    std::vector<Button> buttons_ {};

    const sf::Color backgroundColor_ {18, 20, 28};
    const sf::Color panelColor_ {26, 30, 42};
    const sf::Color panelAccent_ {42, 48, 66};
    const sf::Color boardColor_ {30, 34, 48};
    const sf::Color selectedColor_ {68, 114, 196, 180};
    const sf::Color relatedColor_ {46, 74, 120, 120};
    const sf::Color originalNumberColor_ {232, 236, 244};
    const sf::Color userNumberColor_ {115, 214, 177};
    const sf::Color invalidNumberColor_ {255, 96, 109};
    const sf::Color buttonTextColor_ {235, 238, 247};
    const sf::Color highlightButtonColor_ {65, 104, 214};

    const float windowWidth_ {900.0f};
    const float windowHeight_ {860.0f};
    const float boardLeft_ {90.0f};
    const float boardTop_ {150.0f};
    const float cellSize_ {60.0f};
};

#endif
