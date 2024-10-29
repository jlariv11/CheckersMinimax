#include <iostream>
#include "game.h"

#include <sfml/Window.hpp>
#include "Checker.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "ScreenManager.h"

Game::Game() {
    currentPlayer = RED;
    gameState = IN_PROGRESS;
    movesSinceLastCapture = 0;
    statesSinceLastCapture = std::vector<Board>();
    isJumpingTurn = false;
    jumpingChecker = nullptr;
    currentChecker = nullptr;
}

Game::~Game() = default;


int Game::checkValidMove() {
    if(board.getCheckerAt(currentChecker->getPosition(), NONE) != nullptr) {
        return -1;
    }
    if(isJumpingTurn && currentChecker != jumpingChecker) {
        return -1;
    }
    int deltaX = lastCheckerPosition.x - currentChecker->getPosition().x;
    int deltaY = lastCheckerPosition.y - currentChecker->getPosition().y;
    double angle = atan2(deltaY, deltaX);
    double distance = sqrt(deltaX * deltaX + deltaY * deltaY);
    // Regular move: between 45 and 95
    // Jump move: between 120 and 170
    if(distance > 120 && distance < 170) {
        int xHalf = lastCheckerPosition.x - (deltaX / 2);
        int yHalf = lastCheckerPosition.y - (deltaY/2);
        if(board.getCheckerAt({xHalf, yHalf}, currentPlayer) != nullptr) {
            if(currentChecker->isKing()) {
                if(((angle < -0.7 && angle > -2.5) || (angle > 0.7 && angle < 2.5))) {
                    return 1;
                }
            }else if(currentPlayer == BLACK) {
                if(angle > 0.7 && angle < 2.5) {
                    return 1;
                }
            }else if(currentPlayer == RED) {
                if(angle < -0.7 && angle > -2.5) {
                    return 1;
                }
            }
        }
    }
    if(isJumpingTurn) {
        return -1;
    }
    if(distance > 45 && distance < 95) {
        if(currentChecker->isKing()) {
            if((angle < -0.7 && angle > -2.5) || (angle > 0.7 && angle < 2.5)) {
                return 0;
            }
        }else if(currentPlayer == BLACK) {
            if(angle > 0.7 && angle < 2.5 && distance > 45 && distance < 95) {
                return 0;
            }
        }else if(currentPlayer == RED) {
            if(angle < -0.7 && angle > -2.5 && distance > 45 && distance < 95) {
                return 0;
            }
        }
    }
    return -1;
}

void Game::processMouseClick(const sf::Event& e) {
    if(currentChecker != nullptr) {
        int validMove = checkValidMove();
        if(validMove == 0) {
            board.moveChecker(lastCheckerPosition, currentChecker->getPosition());
        }else if(validMove == 1) {
            int deltaX = lastCheckerPosition.x - currentChecker->getPosition().x;
            int deltaY = lastCheckerPosition.y - currentChecker->getPosition().y;
            board.moveChecker(lastCheckerPosition, currentChecker->getPosition());

            int xHalf = lastCheckerPosition.x - (deltaX / 2);
            int yHalf = lastCheckerPosition.y - (deltaY / 2);

            board.removeChecker({xHalf, yHalf});
            onPieceCapture();
            onCheckerMove(lastCheckerPosition, currentChecker->getPosition(), currentChecker);
            if(hasMoves(currentChecker, true)) {
                isJumpingTurn = true;
                jumpingChecker = currentChecker;
                currentChecker = nullptr;
                return;
            }
        }else {
            board.moveChecker(lastCheckerPosition, lastCheckerPosition);
        }
        if(validMove != -1) {
            isJumpingTurn = false;
            jumpingChecker = nullptr;
            onCheckerMove(lastCheckerPosition, currentChecker->getPosition(), currentChecker);
            onTurnChange();
        }
        currentChecker = nullptr;
        return;
    }
    int posX = e.mouseButton.x;
    int posY = e.mouseButton.y;
    Checker* clickedChecker = board.getCheckerAt({posX, posY}, NONE);
    if(clickedChecker != nullptr) {
        currentChecker = clickedChecker;
        lastCheckerPosition = clickedChecker->getPosition();
        clickedChecker->setPosition({posX, posY});
    }
}

void Game::processMouseMove(const sf::Event& e) const {
    if(currentChecker != nullptr) {
        currentChecker->setPosition({e.mouseMove.x, e.mouseMove.y});
    }
}

void Game::onTurnChange() {
    currentPlayer = getOpposite(currentPlayer);
    movesSinceLastCapture++;
    statesSinceLastCapture.push_back(board);
}

void Game::onCheckerMove(sf::Vector2i from, sf::Vector2i to, Checker *checker) {
    if(checker->getPlayer() == RED && to.y == 475 && !checker->isKing()) {
        checker->setKing();
    }
    if(checker->getPlayer() == BLACK && to.y == 125 && !checker->isKing()) {
        checker->setKing();
    }
    checkGameState();
}


bool Game::checkBounds(int mouseX, int mouseY, Checker* checker) {
    if(mouseX <= checker->getPosition().x + CHECKER_RADIUS && mouseX >= checker->getPosition().x - CHECKER_RADIUS) {
        if(mouseY <= checker->getPosition().y + CHECKER_RADIUS && mouseY >= checker->getPosition().y - CHECKER_RADIUS) {
            return true;
        }
    }
    return false;
}

void Game::run() {
    ScreenManager *screenManager = ScreenManager::getInstance();
    screenManager->start();
    int adjustTime = 0;
    while(screenManager->getWindow()->isOpen()) {
        int startTime = sf::Clock().getElapsedTime().asMilliseconds();
        // Events(Input)
        sf::Event event;
        while(screenManager->getWindow()->pollEvent(event)) {
            if(event.type == sf::Event::Closed) {
                screenManager->getWindow()->close();
                gameState = DRAW;
            }
            if(event.type == sf::Event::MouseButtonPressed) {
                if(event.mouseButton.button == sf::Mouse::Left) {
                    processMouseClick(event);
                }
            }
            if(event.type == sf::Event::MouseMoved) {
                processMouseMove(event);
            }
        }


        // Render
        screenManager->drawScreen(board, currentPlayer, gameState);

        int currentTime = sf::Clock().getElapsedTime().asMilliseconds();
        int deltaTime = currentTime - startTime;
        int intendedSleepTime = 33 - deltaTime - adjustTime;
        if(intendedSleepTime < 0) {
            intendedSleepTime = 0;
        }
        sf::sleep(sf::milliseconds(intendedSleepTime));
        adjustTime = sf::Clock().getElapsedTime().asMilliseconds() - intendedSleepTime;
        if(adjustTime < 0) {
            adjustTime = 0;
        }
    }
}

void Game::onPieceCapture() {
    movesSinceLastCapture = 0;
    statesSinceLastCapture.clear();
}
// Rules based on https://cardgames.io/checkers/

void Game::checkGameState() {
    if(movesSinceLastCapture >= 100){
        gameState = DRAW;
        return;
    }
    std::vector<Board> statesCopy = statesSinceLastCapture;
    if (!statesCopy.empty()) {
        Board currentBoard = statesCopy.back();
        statesCopy.pop_back();
        while(!statesCopy.empty()){
            int sameBoardCount = 0;
            for(Board b : statesSinceLastCapture){
                if(currentBoard != b) {
                    sameBoardCount++;
                    break;
                }
            }
            currentBoard = statesCopy.back();
            statesCopy.pop_back();
            if(sameBoardCount >= 3){
                gameState = DRAW;
                return;
            }
        }
    }

    bool canMove = false;
    for(int i = 0; i < BOARD_WIDTH; i++) {
        for(int j = 0; j < BOARD_HEIGHT; j++) {
            Checker* c = board.getCheckerAtArray({i, j});
            if(c != nullptr) {
                if(c->getPlayer() != getOpposite(currentPlayer)) {
                    continue;
                }
                if(hasMoves(c, false)) {
                    canMove = true;
                    break;
                }
            }
        }
    }
    if(!canMove) {
        gameState = getOpposite(currentPlayer) == RED ? BLACK_WIN : RED_WIN;
    }
    gameState = board.hasWinner();
}

bool Game::hasMoves(Checker* checker, bool onlyJump) {
    int checkerX = checker->getPosition().x;
    int checkerY = checker->getPosition().y;
    std::vector<sf::Vector2i> jumpPositions{};
    std::vector<sf::Vector2i> possibleJumps{};

    if(checker->isKing()) {
        possibleJumps.emplace_back(checkerX + BOARD_SQUARE_SIZE*2, checkerY + BOARD_SQUARE_SIZE*2);
        possibleJumps.emplace_back(checkerX - BOARD_SQUARE_SIZE*2, checkerY + BOARD_SQUARE_SIZE*2);
        possibleJumps.emplace_back(checkerX + BOARD_SQUARE_SIZE*2, checkerY - BOARD_SQUARE_SIZE*2);
        possibleJumps.emplace_back(checkerX - BOARD_SQUARE_SIZE*2, checkerY - BOARD_SQUARE_SIZE*2);

        jumpPositions.emplace_back(checkerX + BOARD_SQUARE_SIZE, checkerY + BOARD_SQUARE_SIZE);
        jumpPositions.emplace_back(checkerX - BOARD_SQUARE_SIZE, checkerY + BOARD_SQUARE_SIZE);
        jumpPositions.emplace_back(checkerX + BOARD_SQUARE_SIZE, checkerY - BOARD_SQUARE_SIZE);
        jumpPositions.emplace_back(checkerX - BOARD_SQUARE_SIZE, checkerY - BOARD_SQUARE_SIZE);
    }else if(checker->getPlayer() == BLACK) {
        possibleJumps.emplace_back(checkerX + BOARD_SQUARE_SIZE*2, checkerY - BOARD_SQUARE_SIZE*2);
        possibleJumps.emplace_back(checkerX - BOARD_SQUARE_SIZE*2, checkerY - BOARD_SQUARE_SIZE*2);
        jumpPositions.emplace_back(checkerX + BOARD_SQUARE_SIZE, checkerY - BOARD_SQUARE_SIZE);
        jumpPositions.emplace_back(checkerX - BOARD_SQUARE_SIZE, checkerY - BOARD_SQUARE_SIZE);
    }else if(checker->getPlayer() == RED) {
        possibleJumps.emplace_back(checkerX + BOARD_SQUARE_SIZE*2, checkerY + BOARD_SQUARE_SIZE*2);
        possibleJumps.emplace_back(checkerX - BOARD_SQUARE_SIZE*2, checkerY + BOARD_SQUARE_SIZE*2);
        jumpPositions.emplace_back(checkerX + BOARD_SQUARE_SIZE, checkerY + BOARD_SQUARE_SIZE);
        jumpPositions.emplace_back(checkerX - BOARD_SQUARE_SIZE, checkerY + BOARD_SQUARE_SIZE);
    }

    for(int j = 0; j < jumpPositions.size(); j++) {
        if(jumpPositions[j].x < 275 || jumpPositions[j].x > 525 || possibleJumps[j].x < 275 || possibleJumps[j].x > 525) {
            continue;
        }
        if(jumpPositions[j].y < 125 || jumpPositions[j].y > 475 || possibleJumps[j].y < 125 || possibleJumps[j].y > 475) {
            continue;
        }
        if(onlyJump) {
            if(board.getCheckerAt(jumpPositions[j], currentPlayer) != nullptr && board.getCheckerAt(possibleJumps[j], NONE) == nullptr) {
                return true;
            }
        }else {
             if(board.getCheckerAt(jumpPositions[j], NONE) == nullptr || (board.getCheckerAt(jumpPositions[j], currentPlayer) != nullptr && board.getCheckerAt(possibleJumps[j], NONE) == nullptr)) {
                 return true;
             }
        }
    }

    return false;
}


// AI is maximizing
// Player is minimizing
void Game::minimax(int** board, int depth, bool isMaximizing) {
    int value = 0;
    //TODO: or someone wins or draws
    if(depth == 0) {

    }
    if(isMaximizing) {
        value = -INFINITY;
        for(int i = 0; i < BOARD_HEIGHT; i++) {
            for(int j = 0; j < BOARD_WIDTH; j++) {
                if(board[i][j] == RED) {
                    continue;
                }
            }
        }
    }
}


int main() {
    Game g;
    g.run();
    return 0;
}