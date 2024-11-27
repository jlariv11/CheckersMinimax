//
// Created by Jake Lariviere on 11/26/24.
//


#include "Board.h"

#include <iostream>

Board::Board() {
    // right-most [] determines the row
    // left-most [] determines the spot within that row (column)

    bool red = true;
    for(int i = 0; i < BOARD_SIZE; i++){
        for(int j = 0; j < BOARD_SIZE; j++){
            board[j][i] = EMPTY_CHECKER;
        }
    }
    for(int i = 0; i < BOARD_SIZE; i++){
        if(i == 3 || i == 4){
            red = false;
            continue;
        }
        for(int j = red; j < BOARD_SIZE; j+=2){
            if(i < 3) {
                board[j][i] = BoardChecker(RED, false, sf::Vector2i(i, j));
            }else {
                board[j][i] = BoardChecker(BLACK, false, sf::Vector2i(i, j));
            }
        }
        red = !red;
    }

}

// X is column
// Y is row
BoardChecker Board::getCheckerAt(sf::Vector2i pos) {
    if(pos.x >= BOARD_SIZE || pos.y >= BOARD_SIZE) {
        return EMPTY_CHECKER;
    }
    return board[pos.x][pos.y];
}

/*
 * Checks if the move is valid.
 * Returns:
 * 0: invalid move
 * 1: valid move (non-jump)
 * 2: valid jump move
 */
int Board::isValidMove(sf::Vector2i from, sf::Vector2i to) {
    // Check if the move is in bounds
    if(to.x >= BOARD_SIZE || to.y >= BOARD_SIZE) {
        std::cout << "Out of Bounds" << std::endl;
        return 0;
    }

    // Check if the move is a valid jump
    sf::Vector2i direction = to - from;
    int magnitude = sqrt(pow(direction.x, 2) + pow(direction.y, 2));
    // A jump will always be 2 units away from the origin point
    // Also need to make sure the checker in the middle space is the opposite of the jumping checker
    BoardChecker movingChecker = getCheckerAt(from);
    BoardChecker jumpedChecker = findJumpedChecker(from, to);
    if(magnitude == 2 && jumpedChecker != EMPTY_CHECKER && jumpedChecker.color != movingChecker.color) {
        std::cout << "Jump" << std::endl;
        return 2;
    }

    // If the move isn't a valid jump, check if it is a valid normal move
    if(getCheckerAt(to) == EMPTY_CHECKER) {
        std::cout << "Normal Move" << std::endl;
        return 1;
    }

    std::cout << "Fail" << std::endl;

    return 0;
}

BoardChecker Board::findJumpedChecker(sf::Vector2i from, sf::Vector2i to) {
    sf::Vector2i direction = to - from;
    direction.x = direction.x > 0 ? 1 : -1;
    direction.y = direction.y > 0 ? 1 : -1;
    sf::Vector2i jumpedChecker = to - direction;
    return board[jumpedChecker.x][jumpedChecker.y];
}



bool Board::move(sf::Vector2i from, sf::Vector2i to) {
    int moveRes = isValidMove(from, to);
    if(moveRes == 0) {
        return false;
    }
    BoardChecker mover = board[from.x][from.y];
    board[from.x][from.y] = EMPTY_CHECKER;
    board[to.x][to.y] = mover;

    BoardChecker jumpedChecker = EMPTY_CHECKER;

    if(moveRes == 2) {
        jumpedChecker = findJumpedChecker(from, to);
    }

    lastMove = LastMove(from, to, jumpedChecker);
    return true;
}

void Board::undoLastMove() {
    BoardChecker mover = board[lastMove.currentPos.x][lastMove.currentPos.y];
    board[lastMove.currentPos.x][lastMove.currentPos.y] = EMPTY_CHECKER;
    board[lastMove.prevPos.x][lastMove.prevPos.y] = mover;
    if(lastMove.jumpedChecker != EMPTY_CHECKER) {
        board[lastMove.jumpedChecker.position.x][lastMove.jumpedChecker.position.y] = lastMove.jumpedChecker;
    }
}

SpaceOccupancy Board::getOpposite(SpaceOccupancy color) {
    switch (color) {
        case RED:
            return BLACK;
        case BLACK:
            return RED;
        default:
            return NONE;
    }
}




