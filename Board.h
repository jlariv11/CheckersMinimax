//
// Created by Jake Lariviere on 10/28/24.
//

#ifndef BOARD_H
#define BOARD_H
#include <vector>

#include "Checker.h"
#include "GameProps.h"

class Board {

/*
 * The Board is responsible for:
 * storing/deleting the checkers
 * viewing the checkers
 * modifying the positions of the checkers
 */

public:
    Board();
    ~Board();
    Board(const Board& other);
    std::shared_ptr<Checker> getCheckerAt(sf::Vector2i pos, Player ignore);
    std::shared_ptr<Checker> getCheckerAtArray(sf::Vector2i pos);
    void moveChecker(sf::Vector2i from, sf::Vector2i to, bool isJump);
    void removeChecker(sf::Vector2i pos);
    sf::Vector2i getClosestPosition(sf::Vector2i pos);
    std::vector<std::shared_ptr<Checker>> getRedCheckers();
    std::vector<std::shared_ptr<Checker>> getBlackCheckers();
    void drawCheckers();
    void undo(sf::Vector2i from, sf::Vector2i to, bool isJump);
    int generateCheckerID();
    void debugPosition(sf::Vector2i pos);
    bool operator==(const Board& other) const;
    bool operator!=(const Board& other) const;

private:
    std::shared_ptr<Checker> board[BOARD_WIDTH][BOARD_HEIGHT];
    std::vector<Checker*> debugCheckers;
    std::vector<std::shared_ptr<Checker>> blackCheckers;
    std::vector<std::shared_ptr<Checker>> redCheckers;
    sf::Vector2i arrayToWorld(sf::Vector2i arrayPos);
    sf::Vector2i worldToArray(sf::Vector2i worldPos);
    sf::Vector2i round(sf::Vector2i vec);
    void deleteChecker(std::shared_ptr<Checker> checker);
};


#endif //BOARD_H
