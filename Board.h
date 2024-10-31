//
// Created by Jake Lariviere on 10/28/24.
//

#ifndef BOARD_H
#define BOARD_H
#include <vector>

#include "Checker.h"
#include "GameProps.h"

class Board {

public:
    Board();
    ~Board();
    Checker* getCheckerAt(sf::Vector2i pos, Player ignore);
    Checker* getCheckerAtArray(sf::Vector2i pos);
    void moveChecker(sf::Vector2i from, sf::Vector2i to, bool isJump);
    void removeChecker(sf::Vector2i pos);
    sf::Vector2i getClosestPosition(sf::Vector2i pos);
    void drawCheckers();
    GameState hasWinner();
    void debugPosition(sf::Vector2i pos);
    bool operator==(const Board& other) const;
    bool operator!=(const Board& other) const;

private:
    Checker* board[BOARD_WIDTH][BOARD_HEIGHT];
    std::vector<Checker*> debugCheckers;
    sf::Vector2i arrayToWorld(sf::Vector2i arrayPos);
    sf::Vector2i worldToArray(sf::Vector2i worldPos);
    sf::Vector2i round(sf::Vector2i vec);
};


#endif //BOARD_H
