//
// Created by Jake Lariviere on 10/28/24.
//

#include "Board.h"

#include <cstdlib>
#include <iostream>

Board::Board() {
    bool red = true;
    for(int i = 0; i < BOARD_HEIGHT; i++){
        for(int j = 0; j < BOARD_WIDTH; j++){
            board[i][j] = nullptr;
        }
    }
    int id = 0;
    for(int i = 0; i < BOARD_HEIGHT; i++){
        if(i == 3 || i == 4){
            red = false;
            continue;
        }
        for(int j = red; j < BOARD_WIDTH; j+=2){
            sf::Vector2i pos = arrayToWorld({j, i});
            board[i][j] = i < 3 ? new Checker(pos, RED, id++) : new Checker(pos, BLACK, id++);
        }
        red = !red;
    }
}

Board::~Board() {
    //TODO: FIX
    /*
    for (int i = 0; i < BOARD_WIDTH; ++i) {
        for(int j = 0; j < BOARD_HEIGHT; j++) {
            if(board[i][j] != nullptr) {
                delete board[i][j];
            }
        }
    }
    */
}

sf::Vector2i Board::arrayToWorld(sf::Vector2i arrayPos) {
    int x = (arrayPos.x * BOARD_SQUARE_SIZE) + 225;
    int y = (arrayPos.y * BOARD_SQUARE_SIZE) + 125;
    return {x,y};
}

void Board::drawCheckers() {
    for(int i = 0; i < BOARD_WIDTH; i++) {
        for(int j = 0; j < BOARD_HEIGHT; j++) {
            if(board[i][j] != nullptr) {
                board[i][j]->draw();
            }
        }
    }
    for(Checker* c : debugCheckers) {
        c->draw();
    }
}


sf::Vector2i Board::worldToArray(sf::Vector2i worldPos) {
    // 225 px is 0th column
    int x = (worldPos.x - 225) / BOARD_SQUARE_SIZE;
    // 125 px is 0th row
    int y = (worldPos.y - 125) / BOARD_SQUARE_SIZE;
    return {x,y};
}

sf::Vector2i Board::round(sf::Vector2i vec) {
    int roundedX = ((vec.x + 12) / 25) * 25;
    if(roundedX % 10 == 0) {
        if(vec.x < roundedX) {
            roundedX -= 25;
        }else {
            roundedX += 25;
        }
    }
    int roundedY = ((vec.y + 12) / 25) * 25;
    if(roundedY % 10 == 0) {
        if(vec.y < roundedY) {
            roundedY -= 25;
        }else {
            roundedY += 25;
        }
    }
    return {roundedX,roundedY};
}

void Board::debugPosition(sf::Vector2i pos) {
    debugCheckers.push_back(new Checker(pos, NONE, -1));
}


sf::Vector2i Board::getClosestPosition(sf::Vector2i pos) {
    return round(pos);
}

Checker* Board::getCheckerAt(sf::Vector2i pos, Player ignore) {
    sf::Vector2i arrayPos = worldToArray(getClosestPosition(pos));
    if(board[arrayPos.y][arrayPos.x] != nullptr && board[arrayPos.y][arrayPos.x]->getPlayer() == ignore) {
        return nullptr;
    }
    return board[arrayPos.y][arrayPos.x];
}

Checker *Board::getCheckerAtArray(sf::Vector2i pos) {
    return board[pos.y][pos.x];
}


GameState Board::hasWinner() {
    int redCount = 0;
    int blackCount = 0;
    for(int i = 0; i < BOARD_HEIGHT; i++) {
        for(int j = 0; j < BOARD_WIDTH; j++) {
            if(board[i][j] != nullptr) {
                if(board[i][j]->getPlayer() == BLACK) {
                    blackCount++;
                }else if(board[i][j]->getPlayer() == RED) {
                    redCount++;
                }
            }
        }
    }
    if(blackCount == 0) {
        return RED_WIN;
    }
    if(redCount == 0) {
        return BLACK_WIN;
    }
    return IN_PROGRESS;
}


void Board::removeChecker(sf::Vector2i pos) {
    sf::Vector2i checkerPos = worldToArray(getClosestPosition(pos));
    free(board[checkerPos.y][checkerPos.x]);
    board[checkerPos.y][checkerPos.x] = nullptr;
}


void Board::moveChecker(sf::Vector2i from, sf::Vector2i to, bool isJump) {
    sf::Vector2i boardFrom = worldToArray(from);
    sf::Vector2i worldClosest = getClosestPosition(to);
    sf::Vector2i boardTo = worldToArray(worldClosest);
    Checker* checker = board[boardFrom.y][boardFrom.x];
    if(checker == nullptr) {
        return;
    }
    board[boardFrom.y][boardFrom.x] = nullptr;
    checker->setPosition(worldClosest);
    board[boardTo.y][boardTo.x] = checker;
    if(isJump) {
        int deltaX = from.x - to.x;
        int deltaY = from.y - to.y;

        int xHalf = from.x - (deltaX / 2);
        int yHalf = from.y - (deltaY / 2);

        removeChecker({xHalf, yHalf});
    }
}

bool Board::operator==(const Board& other) const {
    for(int i = 0; i < BOARD_WIDTH; i++) {
        for(int j = 0; j < BOARD_HEIGHT; j++) {
            if(board[i][j] != other.board[i][j]) {
                return false;
            }
        }
    }
    return true;
}

bool Board::operator!=(const Board &other) const {
    return !(*this == other);
}






