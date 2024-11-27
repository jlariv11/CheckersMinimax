//
// Created by Jake Lariviere on 11/26/24.
//

#include "WorldChecker.h"

#include <set>

#include "ScreenManager.h"
#include "SFML/Graphics/CircleShape.hpp"

WorldChecker::WorldChecker(sf::Vector2i pos, SpaceOccupancy color, bool king) {
    this->pos = pos;
    this->color = color;
    this->king = king;
}

sf::Vector2i WorldChecker::getPos() {
    return this->pos;
}

int WorldChecker::getX() {
    return this->pos.x;
}

int WorldChecker::getY() {
    return this->pos.y;
}

void WorldChecker::setPosition(int x, int y) {
    setPosition({x, y});
}

void WorldChecker::setPosition(sf::Vector2i pos) {
    this->pos = pos;
}

void WorldChecker::setX(int x) {
    this->pos.x = x;
}

void WorldChecker::setY(int y) {
    this->pos.y = y;
}

bool WorldChecker::isKing() {
    return this->king;
}

void WorldChecker::setKing(bool king) {
    this->king = king;
}


void WorldChecker::draw() {
    sf::CircleShape circle(CHECKER_RADIUS);
    circle.setPosition(pos.x, pos.y);
    circle.setFillColor(color == RED ? sf::Color::Red : sf::Color::Black);
    ScreenManager::getInstance()->getWindow()->draw(circle);
}

SpaceOccupancy WorldChecker::getColor() {
    return this->color;
}



