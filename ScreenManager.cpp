//
// Created by Jake Lariviere on 11/24/24.
//

#include "ScreenManager.h"
#include "Constants.h"
#include "SFML/Graphics/RectangleShape.hpp"

ScreenManager::ScreenManager() = default;
ScreenManager::~ScreenManager() = default;

void ScreenManager::start() {
    if(!font.loadFromFile("./pricedown/pricedown.otf")) {
        return;
    }
    window = new sf::RenderWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Checkers Minimax");
}

ScreenManager *ScreenManager::getInstance() {
    static ScreenManager* instance;
    if(instance == nullptr) {
        instance = new ScreenManager();
    }
    return instance;
}

sf::RenderWindow* ScreenManager::getWindow() {
    return window;
}


void ScreenManager::drawScreen(std::vector<WorldChecker*>* checkers) {
    window->clear(sf::Color::White);
    drawBackground();

    for(WorldChecker* checker : *checkers) {
        checker->draw();
    }

    window->display();
}

void ScreenManager::drawBackground() const {
    bool red = true;
    int posX = BOARD_OFFSET_X;
    int posY = BOARD_OFFSET_Y;
    sf::RectangleShape bg(sf::Vector2f(BOARD_SQUARE_SIZE * 9, BOARD_SQUARE_SIZE * 9));
    bg.setPosition(BOARD_OFFSET_X - 25, BOARD_OFFSET_Y - 25);
    bg.setFillColor(sf::Color(97, 54, 2));
    window->draw(bg);
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            sf::RectangleShape rect(sf::Vector2f(BOARD_SQUARE_SIZE, BOARD_SQUARE_SIZE));
            rect.setPosition(posX, posY);
            rect.setFillColor(red ? sf::Color::White : sf::Color::Blue);
            window->draw(rect);
            red = !red;
            posX += BOARD_SQUARE_SIZE;
        }
        posY += BOARD_SQUARE_SIZE;
        posX = BOARD_OFFSET_X;
        red = !red;
    }

}


