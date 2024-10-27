//
// Created by Jake Lariviere on 10/20/24.
//


#include "Checker.h"

#include "ScreenManager.h"
#include "SFML/Graphics/CircleShape.hpp"

Checker::Checker(int id) {
    this->player = NONE;
    this->posX = 0;
    this->posY = 0;
    this->king = false;
    this->checkerID = id;
}

Checker::Checker(Player player, int id) {
    this->player = player;
    this->posX = 0;
    this->posY = 0;
    this->king = false;
    this->checkerID = id;
}

Checker::Checker(int x, int y, Player player, int id) {
    this->player = player;
    this->posX = x;
    this->posY = y;
    this->king = false;
    this->checkerID = id;
}

Checker::~Checker() = default;

int Checker::getID() const {
    return this->checkerID;
}


void Checker::setPosition(int x, int y) {
    posX = x;
    posY = y;
}

sf::Vector2i Checker::getPosition() const {
    return {posX, posY};
}
Player Checker::getPlayer() const {
    return player;
}

bool Checker::isKing() const {
    return king;
}

void Checker::setKing() {
    king = true;
}



void Checker::draw() const {
    sf::CircleShape shape(CHECKER_RADIUS);
    shape.setPosition(posX, posY);
    shape.setFillColor(player == RED ? sf::Color::Red : sf::Color::Black);
    shape.setOrigin(CHECKER_RADIUS, CHECKER_RADIUS);
    ScreenManager::getInstance()->getWindow()->draw(shape);
    if(isKing()) {
        sf::CircleShape shape1(CHECKER_RADIUS - 10);
        shape1.setPosition(posX, posY);
        shape1.setFillColor(sf::Color::Yellow);
        shape1.setOrigin(CHECKER_RADIUS - 10, CHECKER_RADIUS - 10);
        ScreenManager::getInstance()->getWindow()->draw(shape1);
    }

}


