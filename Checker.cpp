//
// Created by Jake Lariviere on 10/20/24.
//


#include "Checker.h"

#include "ScreenManager.h"
#include "SFML/Graphics/CircleShape.hpp"

Checker::Checker() {
    this->player = NONE;
    this->posX = 0;
    this->posY = 0;
    this->king = false;
}

Checker::Checker(Player player) {
    this->player = player;
    this->posX = 0;
    this->posY = 0;
    this->king = false;
}

Checker::Checker(float x, float y, Player player) {
    this->player = player;
    this->posX = x;
    this->posY = y;
    this->king = false;
}

Checker::~Checker() {

}

void Checker::setPosition(float x, float y) {
    posX = x;
    posY = y;
}

sf::Vector2f Checker::getPosition() {
    return sf::Vector2f(posX, posY);
}
Player Checker::getPlayer() {
    return player;
}

bool Checker::isKing() {
    return king;
}


void Checker::draw() {
    sf::CircleShape shape(CHECKER_RADIUS);
    shape.setPosition(posX, posY);
    shape.setFillColor(player == RED ? sf::Color::Red : sf::Color::Black);
    shape.setOrigin(CHECKER_RADIUS, CHECKER_RADIUS);
    ScreenManager::getInstance()->getWindow()->draw(shape);
}


