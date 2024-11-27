//
// Created by Jake Lariviere on 11/26/24.
//

#ifndef BOARD_H
#define BOARD_H
#include "Constants.h"
#include "SFML/System/Vector2.hpp"

enum SpaceOccupancy {
    RED,
    BLACK,
    NONE
};

struct BoardChecker {
    SpaceOccupancy color;
    bool king;
    sf::Vector2i position;

    bool operator==(const BoardChecker & board_checker) const {
        return color == board_checker.color && king == board_checker.king && position == board_checker.position;
    };
};

struct LastMove {
    sf::Vector2i prevPos;
    sf::Vector2i currentPos;
    BoardChecker jumpedChecker;
};

class Board {
/*
 * All positions referenced here will be in board form (8,8) not world form (800, 600)
 */
public:
    Board();
    const BoardChecker EMPTY_CHECKER = BoardChecker(NONE, false, sf::Vector2i());
    BoardChecker getCheckerAt(sf::Vector2i pos);
    bool move(sf::Vector2i from, sf::Vector2i to);
    void undoLastMove();
private:
    BoardChecker board[BOARD_SIZE][BOARD_SIZE];
    LastMove lastMove;
    int isValidMove(sf::Vector2i from, sf::Vector2i to);
    BoardChecker findJumpedChecker(sf::Vector2i from, sf::Vector2i to);
    SpaceOccupancy getOpposite(SpaceOccupancy color);


};

#endif //BOARD_H
