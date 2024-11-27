//
// Created by Jake Lariviere on 11/26/24.
//

#ifndef CHECKER_H
#define CHECKER_H
#include "Board.h"
#include "SFML/System/Vector2.hpp"

class WorldChecker {

public:
    WorldChecker(sf::Vector2i pos, SpaceOccupancy color, bool king);
    ~WorldChecker();
    sf::Vector2i getPos();
    int getX();
    int getY();
    SpaceOccupancy getColor();
    void setPosition(sf::Vector2i pos);
    void setPosition(int x, int y);
    void setX(int x);
    void setY(int y);
    void setKing(bool value);
    bool isKing();
    void draw();

private:
    SpaceOccupancy color;
    sf::Vector2i pos;
    bool king;
};

#endif //CHECKER_H
