//
// Created by Jake Lariviere on 10/20/24.
//


#include "Checker.h"

#include "ScreenManager.h"
#include "SFML/Graphics/CircleShape.hpp"

Checker::Checker(int id) {
    this->player = NONE;
    this->position = sf::Vector2i(0, 0);
    this->king = false;
    this->checkerID = id;
}

Checker::Checker(Player player, int id) {
    this->player = player;
    this->position = sf::Vector2i(0, 0);
    this->king = false;
    this->checkerID = id;
}

Checker::Checker(sf::Vector2i pos, Player player, int id) {
    this->player = player;
    this->position = pos;
    this->king = false;
    this->checkerID = id;
}

Checker::~Checker() = default;

int Checker::getID() const {
    return this->checkerID;
}


void Checker::setPosition(sf::Vector2i pos) {
    this->position = pos;
}

sf::Vector2i Checker::getPosition() const {
    return position;
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
    shape.setPosition(position.x, position.y);
    if(player == NONE) {
        shape.setFillColor(sf::Color::Green);
    }else {
        shape.setFillColor(player == RED ? sf::Color::Red : sf::Color::Black);
    }
    shape.setOrigin(CHECKER_RADIUS, CHECKER_RADIUS);
    ScreenManager::getInstance()->getWindow()->draw(shape);
    if(isKing()) {
        sf::CircleShape shape1(CHECKER_RADIUS - 10);
        shape1.setPosition(position.x, position.y);
        shape1.setFillColor(sf::Color::Yellow);
        shape1.setOrigin(CHECKER_RADIUS - 10, CHECKER_RADIUS - 10);
        ScreenManager::getInstance()->getWindow()->draw(shape1);
    }

}

bool Checker::operator!=(const Checker &other) const {
    return !(*this == other);
}

bool Checker::operator==(const Checker &other) const {
    return checkerID == other.checkerID;
}




