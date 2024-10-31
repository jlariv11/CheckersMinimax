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
    // Create a temporary checker with the current checker's position before being picked up
    Checker* temp = new Checker(lastCheckerPosition, currentChecker->getPlayer(), -1);
    if(currentChecker->isKing()) {
        temp->setKing();
    }
    // Generate all possible moves that could be made from that position
    std::vector<sf::Vector2i> moves = getMoves(temp, false);
    delete temp;
    sf::Vector2i pos = board.getClosestPosition(currentChecker->getPosition());
    // Loop through all moves and see if the current checker's position matches any of them
    for(sf::Vector2i move : moves) {
        if(move == pos) {
            // if it does, return 1(true) if it was a jump move or 0(false) if it was a regular move
            return (move.x - BOARD_SQUARE_SIZE*2) == lastCheckerPosition.x || (move.x + BOARD_SQUARE_SIZE*2) == lastCheckerPosition.x;
        }
    }
    // The player made an invalid move
    return -1;
}

void Game::processMouseClick(const sf::Event& e) {
    if(currentChecker != nullptr) {
        int validMove = checkValidMove();
        if(validMove == -1) {
            board.moveChecker(lastCheckerPosition, lastCheckerPosition, false);
            currentChecker = nullptr;
            return;
        }
        board.moveChecker(lastCheckerPosition, currentChecker->getPosition(), validMove);
        onCheckerMove(lastCheckerPosition, currentChecker->getPosition(), currentChecker);
        if(validMove == 1) {
            onPieceCapture();
            if(hasMoves(currentChecker, true)) {
                isJumpingTurn = true;
                jumpingChecker = currentChecker;
                currentChecker = nullptr;
                return;
            }
        }
        isJumpingTurn = false;
        jumpingChecker = nullptr;
        onTurnChange();
        currentChecker = nullptr;
        return;
    }

    int posX = e.mouseButton.x;
    int posY = e.mouseButton.y;
    Checker* clickedChecker = board.getCheckerAt({posX, posY}, NONE);
    if(clickedChecker != nullptr) {
        if(clickedChecker->getPlayer() != currentPlayer) {
            return;
        }
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
    if(currentPlayer == BLACK) {
        //aiTurn();
    }
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


bool Game::hasMoves(Checker *checker, bool onlyJump) {
    return !getMoves(checker, onlyJump).empty();
}


std::vector<sf::Vector2i> Game::getMoves(Checker* checker, bool onlyJump) {
    int checkerX = checker->getPosition().x;
    int checkerY = checker->getPosition().y;
    std::vector<sf::Vector2i> jumpPositions{};
    std::vector<sf::Vector2i> possibleJumps{};
    std::vector<sf::Vector2i> moves{};

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
        if(jumpPositions[j].x < 175 || jumpPositions[j].x > 625 || possibleJumps[j].x < 175 || possibleJumps[j].x > 625) {
            continue;
        }
        if(jumpPositions[j].y < 100 || jumpPositions[j].y > 500 || possibleJumps[j].y < 100 || possibleJumps[j].y > 500) {
            continue;
        }
        if(onlyJump) {
            if(board.getCheckerAt(jumpPositions[j], currentPlayer) != nullptr && board.getCheckerAt(possibleJumps[j], NONE) == nullptr) {
                moves.push_back(possibleJumps[j]);
            }
        }else {
             if(board.getCheckerAt(jumpPositions[j], NONE) == nullptr) {
                 moves.push_back(jumpPositions[j]);
             }else if (board.getCheckerAt(jumpPositions[j], currentPlayer) != nullptr && board.getCheckerAt(possibleJumps[j], NONE) == nullptr) {
                 moves.push_back(possibleJumps[j]);
             }
        }
    }

    return moves;
}

void Game::aiTurn() {
    Board b = board;
    sf::Vector2i bestMove = sf::Vector2i(0, 0);
    Checker* bestChecker = nullptr;
    int bestScore = 0;
    for(int i = 0; i < BOARD_HEIGHT; i++) {
        for(int j = 0; j < BOARD_WIDTH; j++) {
            Checker* checker = b.getCheckerAtArray({i,j});
            if(checker == nullptr || checker->getPlayer() == RED) {
                continue;
            }
            std::vector<sf::Vector2i> moves = getMoves(checker, false);
            for(sf::Vector2i m : moves) {
                sf::Vector2i lastPos = checker->getPosition();
                bool isJumpMove = (m.x - BOARD_SQUARE_SIZE*2) == lastPos.x || (m.x + BOARD_SQUARE_SIZE*2) == lastPos.x;
                b.moveChecker(lastPos, m, isJumpMove);
                int score = minimax(b, 2, 0, false);
                b.moveChecker(m, lastPos, false);
                if(score > bestScore) {
                    bestScore = score;
                    bestChecker = checker;
                    bestMove = m;
                }
            }
        }
    }
    if(bestChecker != nullptr) {
        sf::Vector2i lastPos = bestChecker->getPosition();
        bool isJumpMove = (bestMove.x - BOARD_SQUARE_SIZE*2) == lastPos.x || (bestMove.x + BOARD_SQUARE_SIZE*2) == lastPos.x;
        board.moveChecker(lastPos, bestMove, isJumpMove);
        onCheckerMove(lastPos, bestMove, bestChecker);
        if(isJumpMove) {
            onPieceCapture();
        }
        onTurnChange();
    }
}


// AI is maximizing
// Player is minimizing
int Game::minimax(Board board, int depth, int score, bool isMaximizing) {
    /*
     * Score:
     * +10 AI capture
     * -10 Player capture
     * +5 AI king
     * -5 Player king
     */
    //TODO: or draws
    if(board.hasWinner() != IN_PROGRESS) {
        if(isMaximizing) {
            return board.hasWinner() == RED_WIN ? (-INFINITY) : (1000 - depth);
        }else {
            return board.hasWinner() == BLACK_WIN ? (INFINITY) : (-1000 + depth);
        }
    }
    if(depth <= 0) {
        return score;
    }
    for(int i = 0; i < BOARD_HEIGHT; i++) {
        for(int j = 0; j < BOARD_WIDTH; j++) {
            Checker* checker = board.getCheckerAtArray({i,j});
            if(checker == nullptr || checker->getPlayer() == (isMaximizing ? RED : BLACK)) {
                continue;
            }
            std::vector<sf::Vector2i> moves = getMoves(checker, false);
            for(sf::Vector2i m : moves) {
                sf::Vector2i lastPos = checker->getPosition();
                bool isJumpMove = (m.x - BOARD_SQUARE_SIZE*2) == lastPos.x || (m.x + BOARD_SQUARE_SIZE*2) == lastPos.x;
                board.moveChecker(lastPos, m, isJumpMove);
                int newScore = score;
                if(isJumpMove) {
                    if(isMaximizing) {
                        newScore += 10;
                    }else {
                        newScore -= 10;
                    }
                }
                if(!checker->isKing()) {
                    if(isMaximizing && m.y == 125) {
                        newScore += 5;
                    }else if(!isMaximizing && m.y == 425) {
                        newScore -= 5;
                    }
                }
                minimax(board, depth - 1, newScore, !isMaximizing);
                board.moveChecker(m, lastPos, false);
            }
        }
    }
}


int main() {
    Game g;
    g.run();
    return 0;
}