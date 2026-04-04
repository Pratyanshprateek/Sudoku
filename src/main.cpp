#include "Game.h"

#include <iostream>

int main() {
    try {
        Game game;
        game.run();
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "Fatal error: " << exception.what() << '\n';
        return 1;
    }
}
