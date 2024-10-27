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
    Checker(int x, int y, Player player, int id);
    ~Checker();
    void draw() const;
    void setPosition(int x, int y);
    bool isKing() const;
    void setKing();
    int getID() const;
    sf::Vector2i getPosition() const;
    Player getPlayer() const;

private:
    int checkerID;
    Player player;
    int posX;
    int posY;
    bool king;
};

#endif //CHECKER_H
