//
// Created by Jake Lariviere on 10/28/24.
//

#include "Board.h"

#include <cstdlib>
#include <iostream>

Board::Board() {
    blackCheckers = std::vector<std::shared_ptr<Checker>>();
    redCheckers = std::vector<std::shared_ptr<Checker>>();
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
            board[i][j] = i < 3 ? std::make_shared<Checker>(pos, RED, id++) : std::make_shared<Checker>(pos, BLACK, id++);
            if(board[i][j]->getPlayer() == RED) {
                redCheckers.push_back(board[i][j]);
            }else {
                blackCheckers.push_back(board[i][j]);
            }
        }
        red = !red;
    }
}

Board::~Board() {
    for(Checker* c : debugCheckers) {
        delete c;
    }
}

Board::Board(const Board &other) {
    for (int i = 0; i < BOARD_HEIGHT; ++i) {
        for (int j = 0; j < BOARD_WIDTH; ++j) {
            std::shared_ptr<Checker> c = other.board[i][j];
            if(c != nullptr) {
                board[i][j] = std::make_shared<Checker>(c->getPosition(), c->getPlayer(), c->getID());
                if(c->getPlayer() == RED) {
                    redCheckers.push_back(board[i][j]);
                }else {
                    blackCheckers.push_back(board[i][j]);
                }
            }else {
                board[i][j] = nullptr;
            }
        }
    }

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
        std::cout << "Drawing" << std::endl;
        c->draw();
    }
}

std::vector<std::shared_ptr<Checker>> Board::getBlackCheckers() {
    return blackCheckers;
}

std::vector<std::shared_ptr<Checker>> Board::getRedCheckers() {
    return redCheckers;
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

std::shared_ptr<Checker> Board::getCheckerAt(sf::Vector2i pos, Player ignore) {
    sf::Vector2i arrayPos = worldToArray(getClosestPosition(pos));
    if(board[arrayPos.y][arrayPos.x] != nullptr && board[arrayPos.y][arrayPos.x]->getPlayer() == ignore) {
        return nullptr;
    }
    return board[arrayPos.y][arrayPos.x];
}

std::shared_ptr<Checker> Board::getCheckerAtArray(sf::Vector2i pos) {
    return board[pos.y][pos.x];
}

void Board::deleteChecker(std::shared_ptr<Checker> checker) {
    if(checker == nullptr) {
        return;
    }
    sf::Vector2i checkerPos = worldToArray(checker->getPosition());
    if(board[checkerPos.y][checkerPos.x]->getPlayer() == BLACK) {
        for(int i = 0; i < blackCheckers.size(); i++) {
            if(blackCheckers[i]->getID() == board[checkerPos.y][checkerPos.x]->getID()) {
                blackCheckers.erase(blackCheckers.begin() + i);
                break;
            }
        }
    }else {
        for(int i = 0; i < redCheckers.size(); i++) {
            if(redCheckers[i]->getID() == board[checkerPos.y][checkerPos.x]->getID()) {
                redCheckers.erase(redCheckers.begin() + i);
                break;
            }
        }
    }
    board[checkerPos.y][checkerPos.x] = nullptr;
}


void Board::removeChecker(sf::Vector2i pos) {
    sf::Vector2i checkerPos = worldToArray(getClosestPosition(pos));
    deleteChecker(board[checkerPos.y][checkerPos.x]);
}

int Board::generateCheckerID() {
    std::vector<std::shared_ptr<Checker>> checkers;
    checkers.reserve(redCheckers.size() + blackCheckers.size());

    // Insert elements from vec1 and vec2
    checkers.insert(checkers.end(), redCheckers.begin(), redCheckers.end());
    checkers.insert(checkers.end(), blackCheckers.begin(), blackCheckers.end());

    int availableID = 0;
    bool isDiff = false;
    while(!isDiff) {
        isDiff = true;
        for(std::shared_ptr<Checker> checker : checkers) {
            if(availableID == checker->getID()) {
                availableID++;
                isDiff = false;
            }
        }
    }
    return availableID;
}


void Board::undo(sf::Vector2i from, sf::Vector2i to, bool isJump) {
    sf::Vector2i boardFrom = worldToArray(from);
    Player enemyChecker = getOpposite(board[boardFrom.y][boardFrom.x]->getPlayer());
    moveChecker(from, to, false);
    if(isJump) {
        int deltaX = from.x - to.x;
        int deltaY = from.y - to.y;
        int xHalf = from.x - (deltaX / 2);
        int yHalf = from.y - (deltaY / 2);
        sf::Vector2i worldPos = getClosestPosition({xHalf, yHalf});
        sf::Vector2i boardPos = worldToArray(worldPos);
        std::shared_ptr<Checker> checker = std::make_shared<Checker>(worldPos, enemyChecker, generateCheckerID());
        board[boardPos.y][boardPos.x] = checker;
        if(enemyChecker == BLACK) {
            blackCheckers.push_back(checker);
        }else {
            redCheckers.push_back(checker);
        }

    }
}

void Board::moveChecker(sf::Vector2i from, sf::Vector2i to, bool isJump) {
    sf::Vector2i boardFrom = worldToArray(from);
    sf::Vector2i worldClosest = getClosestPosition(to);
    sf::Vector2i boardTo = worldToArray(worldClosest);
    std::shared_ptr<Checker> checker = board[boardFrom.y][boardFrom.x];
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






