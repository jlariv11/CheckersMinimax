//
// Created by Jake Lariviere on 10/20/24.
//

#include "ScreenManager.h"

#include <iostream>

#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/RectangleShape.hpp"

ScreenManager::ScreenManager() {

}

ScreenManager::~ScreenManager() {

}

ScreenManager *ScreenManager::getInstance() {
    static ScreenManager* instance;
    if(instance == nullptr) {
        instance = new ScreenManager();
    }
    return instance;
}

void ScreenManager::start() {
    sf::Font font;
    if(!font.loadFromFile("./pricedown/pricedown.otf")) {
        return;
    }
    window = new sf::RenderWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Checkers Minimax");
    drawScreen(std::vector<Checker*>());
}

sf::RenderWindow *ScreenManager::getWindow() {
    return window;
}

void ScreenManager::drawScreen(std::vector<Checker*> checkers) {
    window->clear(sf::Color::White);
    drawBackground();
    drawCheckers(checkers);
    window->display();
}

void ScreenManager::drawCheckers(std::vector<Checker*> checkers) {
    for(int i = 0; i < checkers.size(); i++) {
        checkers[i]->draw();
    }
}


void ScreenManager::drawBackground() {
    bool red = true;
    int posX = BOARD_OFFSET_X;
    int posY = BOARD_OFFSET_Y;
    sf::RectangleShape bg(sf::Vector2f(BOARD_SQUARE_SIZE * 9, BOARD_SQUARE_SIZE * 9));
    bg.setPosition(BOARD_OFFSET_X - 25, BOARD_OFFSET_Y - 25);
    bg.setFillColor(sf::Color(97, 54, 2));
    window->draw(bg);
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            sf::RectangleShape rect(sf::Vector2f(BOARD_SQUARE_SIZE, BOARD_SQUARE_SIZE));
            rect.setPosition(sf::Vector2f(posX, posY));
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

