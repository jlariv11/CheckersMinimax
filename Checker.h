//
// Created by Jake Lariviere on 10/20/24.
//

#ifndef CHECKER_H
#define CHECKER_H
#include "GameProps.h"
#include "SFML/System/Vector2.hpp"

class Checker {
    public:
    Checker(int id);
    Checker(Player player, int id);
    Checker(float x, float y, Player player, int id);
    ~Checker();
    void draw();
    void setPosition(float x, float y);
    bool isKing();
    int getID();
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
