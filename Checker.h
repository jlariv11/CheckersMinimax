//
// Created by Jake Lariviere on 10/20/24.
//

#ifndef CHECKER_H
#define CHECKER_H
#include "GameProps.h"
#include "SFML/System/Vector2.hpp"

class Checker {
    public:
    Checker();
    Checker(Player player);
    Checker(float x, float y, Player player);
    ~Checker();
    void draw();
    void setPosition(float x, float y);
    bool isKing();
    sf::Vector2f getPosition();
    Player getPlayer();

private:
    int checkerID;
    Player player;
    float posX;
    float posY;
    bool king;
};

#endif //CHECKER_H
