#include "game.h"

#include <iostream>

#include "ScreenManager.h"
#include "SFML/Window/Event.hpp"

Game::Game() {
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            BoardChecker bc = board.getCheckerAt({j, i});
            if(bc.color == NONE) {
                continue;
            }
            worldCheckers.push_back(new WorldChecker(boardToWorld({j, i}), bc.color, bc.king));
        }
    }
}

void Game::onMouseClick(sf::Event &event) {
    // Place
    if(clickedChecker != nullptr) {
        if(board.move(worldToBoard(clickedOriginalPos), worldToBoard(clickedChecker->getPos()))) {
            clickedChecker->setPosition(normalizeWorldPos(clickedChecker->getPos()));
        }else {
            clickedChecker->setPosition(clickedOriginalPos);
        }
        clickedChecker = nullptr;
    }
    // Pick Up
    else {
        int clickX = event.mouseButton.x;
        int clickY = event.mouseButton.y;
        sf::Vector2i boardClick = worldToBoard({clickX, clickY});
        BoardChecker bc = board.getCheckerAt(boardClick);
        // A checker was clicked
        if(bc.color != NONE) {
            sf::Vector2i worldClick = boardToWorld(boardClick);
            for(WorldChecker* w : worldCheckers) {
                if(w->getPos() == worldClick) {
                    clickedChecker = w;
                    clickedOriginalPos = worldClick;
                    break;
                }
            }
        }
    }
}

void Game::onMouseMove(sf::Event &event) {
    if(clickedChecker != nullptr) {
        clickedChecker->setPosition(event.mouseMove.x - CHECKER_RADIUS, event.mouseMove.y - CHECKER_RADIUS);
    }
}



sf::Vector2i Game::boardToWorld(sf::Vector2i board) {
    int worldX = BOARD_OFFSET_X + (board.x * BOARD_SQUARE_SIZE);
    int worldY = BOARD_OFFSET_Y + (board.y * BOARD_SQUARE_SIZE);
    return {worldX, worldY};
}

sf::Vector2i Game::worldToBoard(sf::Vector2i world) {
    int boardX = (world.x - BOARD_OFFSET_X) / BOARD_SQUARE_SIZE;
    int boardY = (world.y - BOARD_OFFSET_Y) / BOARD_SQUARE_SIZE;
    return {boardX, boardY};
}

/*
 * Takes a non-exact world position and converts it to an exact one
 * An exact world position is a position where a checker can be placed
 * EX: converts {202, 10} to {200, 0}
 */
sf::Vector2i Game::normalizeWorldPos(sf::Vector2i world) {
    return boardToWorld(worldToBoard(world));
}


void Game::run() {
    ScreenManager* screenManager = ScreenManager::getInstance();
    screenManager->start();

    while(screenManager->getInstance()->getWindow()->isOpen()) {
        // Events(Input)
        sf::Event event;
        while(screenManager->getWindow()->pollEvent(event)) {
            if(event.type == sf::Event::Closed) {
                screenManager->getWindow()->close();
            }
            if(event.type == sf::Event::MouseButtonPressed) {
                if(event.mouseButton.button == sf::Mouse::Left) {
                    onMouseClick(event);
                }
            }
            if(event.type == sf::Event::MouseMoved) {
                onMouseMove(event);
            }
        }

        screenManager->drawScreen(&worldCheckers);
    }
}


int main() {
    Game g;
    g.run();
    return 0;
}
