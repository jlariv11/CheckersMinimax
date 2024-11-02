//
// Created by Jake Lariviere on 10/20/24.
//

#ifndef CHECKER_H
#define CHECKER_H
#include "GameProps.h"
#include "SFML/System/Vector2.hpp"

class Checker {
    /*
     * Checker creates and draws the object to be interacted with and viewed by the player
     * A checker is defined by:
     * its position
     * its id
     * its king status
     * the player it belongs to
     */
    public:
    Checker(int id);
    Checker(Player player, int id);
    Checker(sf::Vector2i pos, Player player, int id);
    ~Checker();
    void draw() const;
    void setPosition(sf::Vector2i position);
    bool isKing() const;
    void setKing();
    int getID() const;
    sf::Vector2i getPosition() const;
    Player getPlayer() const;
    bool operator==(const Checker& other) const;
    bool operator!=(const Checker& other) const;

private:
    int checkerID;
    Player player;
    sf::Vector2i position;
    bool king;
};

#endif //CHECKER_H
