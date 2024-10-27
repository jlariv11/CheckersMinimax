//
// Created by Jake Lariviere on 10/20/24.
//

#include "ScreenManager.h"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Text.hpp"

ScreenManager::ScreenManager() = default;

ScreenManager::~ScreenManager() = default;

ScreenManager *ScreenManager::getInstance() {
    static ScreenManager* instance;
    if(instance == nullptr) {
        instance = new ScreenManager();
    }
    return instance;
}

void ScreenManager::start() {
    if(!font.loadFromFile("./pricedown/pricedown.otf")) {
        return;
    }
    window = new sf::RenderWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Checkers Minimax");
    drawScreen(std::vector<Checker*>(), NONE, STOPPED);
}

sf::RenderWindow *ScreenManager::getWindow() const {
    return window;
}

void ScreenManager::drawScreen(const std::vector<Checker*>& checkers, Player currentPlayer, GameState gameState) const {
    window->clear(sf::Color::White);
    drawBackground();
    drawCheckers(checkers);
    drawInfo(currentPlayer, gameState);
    window->display();
}

void ScreenManager::drawInfo(Player currentPlayer, GameState gameState) const {
    sf::Text text;
    text.setFont(font);
    switch(gameState) {
        case IN_PROGRESS:
            text.setString(currentPlayer == RED ? "Red's Turn" : "Black's Turn");
        break;
        case RED_WIN:
            text.setString("Red Wins!");
        break;
        case BLACK_WIN:
            text.setString("Black Wins!");
        break;
        case DRAW:
            text.setString("Draw!");
        break;
        default:
            text.setString("");
        break;
    }
    text.setPosition(WINDOW_WIDTH / 2, 30);
    text.setCharacterSize(40);
    text.setOrigin(text.getLocalBounds().width / 2, text.getLocalBounds().height / 2);
    text.setFillColor(sf::Color::Black);
    window->draw(text);

}


void ScreenManager::drawCheckers(const std::vector<Checker*> &checkers) {
    for(auto checker : checkers) {
        checker->draw();
    }
}


void ScreenManager::drawBackground() const {
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

