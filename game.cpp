#include <iostream>
#include "game.h"

#include <sfml/Window.hpp>
#include "Checker.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "ScreenManager.h"

Game::Game() {
    board = static_cast<int **>(malloc(sizeof(int*) * BOARD_HEIGHT));
    for (int i = 0; i < BOARD_HEIGHT; ++i) {
        board[i] = static_cast<int *>(malloc(sizeof(int) * BOARD_WIDTH));
    }
    currentPlayer = RED;
    gameState = IN_PROGRESS;
    movesSinceLastCapture = 0;
    statesSinceLastCapture = std::vector<int**>();
    boardPositions = std::vector<sf::Vector2i>(32);
    checkers = std::vector<Checker*>(24);
    isJumpingTurn = false;
    jumpingChecker = nullptr;
    currentChecker = nullptr;
}

Game::~Game() {
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        free(board[i]);
    }
    free(board);
}

void Game::initializeBoard() {
    bool red = true;
    for(int i = 0; i < BOARD_HEIGHT; i++){
        for(int j = 0; j < BOARD_WIDTH; j++){
            board[i][j] = NONE;
        }
    }

    for(int i = 0; i < BOARD_HEIGHT; i++){
        if(i == 3 || i == 4){
            red = false;
            continue;
        }
        for(int j = red; j < BOARD_WIDTH; j+=2){
            board[i][j] = i < 3 ? RED : BLACK;
        }
        red = !red;
    }


    bool corner = false;
    int posX = BOARD_OFFSET_X + BOARD_SQUARE_SIZE*1.5;
    int posY = BOARD_OFFSET_Y + BOARD_SQUARE_SIZE/2;
    for(int i = 0; i < boardPositions.size(); i++) {
        if(i != 0 && i % 4 == 0) {
            corner = !corner;
            posY += BOARD_SQUARE_SIZE;
            posX = (BOARD_OFFSET_X + BOARD_SQUARE_SIZE/2) + (corner ? 0 : BOARD_SQUARE_SIZE);
        }
        boardPositions[i] = sf::Vector2i(posX, posY);
        std::cout << boardPositions[i].x << " " << boardPositions[i].y << std::endl;
        posX += BOARD_SQUARE_SIZE * 2;
    }

    for(int i = 0; i < 12; i++) {
        checkers[i] = new Checker(boardPositions[i].x, boardPositions[i].y, RED, i);
    }
    for(int i = 20; i < 32; i++) {
        checkers[i-8] = new Checker(boardPositions[i].x, boardPositions[i].y, BLACK, i-8);
    }
}
int Game::round(int num) {
    int rounded = ((num + 12) / 25) * 25;
    if(rounded % 10 == 0) {
        if(num < rounded) {
            rounded -= 25;
        }else {
            rounded += 25;
        }
    }
    return rounded;
}

sf::Vector2i Game::getClosestPosition(Checker* checker) {
    int x = round(checker->getPosition().x);
    int y = round(checker->getPosition().y);
    return {x, y};
}

bool Game::checkerAt(int x, int y, Player color) const {
    int xNormal = round(x);
    int yNormal = round(y);
    int* boardPos = worldToBoard(sf::Vector2i(xNormal, yNormal));
    if(boardPos[0] > 7 || boardPos[1] > 7) {
        free(boardPos);
        return true;
    }
    if(board[boardPos[1]][boardPos[0]] == NONE) {
        free(boardPos);
        return false;
    }
    if(board[boardPos[1]][boardPos[0]] != color){
        free(boardPos);
        return true;
    }
    return false;
}

int Game::checkValidMove() const {
    if(checkerAt(currentChecker->getPosition().x, currentChecker->getPosition().y, NONE)) {
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
        if(currentPlayer == BLACK && !currentChecker->isKing()) {
            if(angle > 0.7 && angle < 2.5 && checkerAt(xHalf, yHalf, currentPlayer)) {
                return 1;
            }
        }else if(currentPlayer == RED && !currentChecker->isKing()) {
            if(angle < -0.7 && angle > -2.5 && checkerAt(xHalf, yHalf, currentPlayer)) {
                return 1;
            }
        }else if(currentChecker->isKing()) {
            if(((angle < -0.7 && angle > -2.5) || (angle > 0.7 && angle < 2.5)) && checkerAt(xHalf, yHalf, currentPlayer)) {
                return 1;
            }
        }
    }
    if(isJumpingTurn) {
        return -1;
    }
    if(currentPlayer == BLACK && !currentChecker->isKing()) {
        if(angle > 0.7 && angle < 2.5 && distance > 45 && distance < 95) {
            return 0;
        }
    }else if(currentPlayer == RED && !currentChecker->isKing()) {
        if(angle < -0.7 && angle > -2.5 && distance > 45 && distance < 95) {
            return 0;
        }
    }else if(currentChecker->isKing()) {
        if(((angle < -0.7 && angle > -2.5) || (angle > 0.7 && angle < 2.5)) && distance > 45 && distance < 95) {
            return 0;
        }
    }
    return -1;
}

int* Game::worldToBoard(sf::Vector2i coordinate) {
    // 225 px is 0th column
    int x = (coordinate.x - 225) / BOARD_SQUARE_SIZE;
    // 125 px is 0th row
    int y = (coordinate.y - 125) / BOARD_SQUARE_SIZE;
    int* board = static_cast<int *>(malloc(sizeof(int) * 2));
    board[0] = x;
    board[1] = y;
    return board;
}


void Game::processMouseClick(const sf::Event& e) {
    if(currentChecker != nullptr) {
        int validMove = checkValidMove();
        if(validMove == 0) {
            sf::Vector2i pos = getClosestPosition(currentChecker);
            currentChecker->setPosition(pos.x, pos.y);
            int* prevPos = worldToBoard(lastCheckerPosition);
            int* currentPos = worldToBoard(pos);

            board[prevPos[1]][prevPos[0]] = NONE;
            board[currentPos[1]][currentPos[0]] = currentChecker->getPlayer();

            free(prevPos);
            free(currentPos);

        }else if(validMove == 1) {
            int deltaX = lastCheckerPosition.x - currentChecker->getPosition().x;
            int deltaY = lastCheckerPosition.y - currentChecker->getPosition().y;
            sf::Vector2i pos = getClosestPosition(currentChecker);
            currentChecker->setPosition(pos.x, pos.y);
            int* prevPos = worldToBoard(lastCheckerPosition);
            int* currentPos = worldToBoard(pos);

            board[prevPos[1]][prevPos[0]] = NONE;
            board[currentPos[1]][currentPos[0]] = currentChecker->getPlayer();

            int xHalf = lastCheckerPosition.x - (deltaX / 2);
            int yHalf = lastCheckerPosition.y - (deltaY/2);

            int* checkerToRemove = worldToBoard(sf::Vector2i(round(xHalf), round(yHalf)));

            board[checkerToRemove[1]][checkerToRemove[0]] = NONE;
            for(int i = 0; i < checkers.size(); i++) {
                if(checkers[i]->getPosition().x == round(xHalf) && checkers[i]->getPosition().y == round(yHalf)) {
                    checkers.erase(checkers.begin() + i);
                    onPieceCapture();
                    onCheckerMove(lastCheckerPosition, currentChecker->getPosition(), currentChecker);
                    int checkerX = currentChecker->getPosition().x;
                    int checkerY = currentChecker->getPosition().y;
                    std::vector<sf::Vector2i> jumpPositions{};
                    std::vector<sf::Vector2i> possibleJumps{};

                    if(currentChecker->isKing()) {
                        possibleJumps.emplace_back(checkerX + BOARD_SQUARE_SIZE*2, checkerY + BOARD_SQUARE_SIZE*2);
                        possibleJumps.emplace_back(checkerX - BOARD_SQUARE_SIZE*2, checkerY + BOARD_SQUARE_SIZE*2);
                        possibleJumps.emplace_back(checkerX + BOARD_SQUARE_SIZE*2, checkerY - BOARD_SQUARE_SIZE*2);
                        possibleJumps.emplace_back(checkerX - BOARD_SQUARE_SIZE*2, checkerY - BOARD_SQUARE_SIZE*2);

                        jumpPositions.emplace_back(checkerX + BOARD_SQUARE_SIZE, checkerY + BOARD_SQUARE_SIZE);
                        jumpPositions.emplace_back(checkerX - BOARD_SQUARE_SIZE, checkerY + BOARD_SQUARE_SIZE);
                        jumpPositions.emplace_back(checkerX + BOARD_SQUARE_SIZE, checkerY - BOARD_SQUARE_SIZE);
                        jumpPositions.emplace_back(checkerX - BOARD_SQUARE_SIZE, checkerY - BOARD_SQUARE_SIZE);
                    }else if(currentChecker->getPlayer() == BLACK) {
                        possibleJumps.emplace_back(checkerX + BOARD_SQUARE_SIZE*2, checkerY - BOARD_SQUARE_SIZE*2);
                        possibleJumps.emplace_back(checkerX - BOARD_SQUARE_SIZE*2, checkerY - BOARD_SQUARE_SIZE*2);
                        jumpPositions.emplace_back(checkerX + BOARD_SQUARE_SIZE, checkerY - BOARD_SQUARE_SIZE);
                        jumpPositions.emplace_back(checkerX - BOARD_SQUARE_SIZE, checkerY - BOARD_SQUARE_SIZE);
                    }else if(currentChecker->getPlayer() == RED) {
                        possibleJumps.emplace_back(checkerX + BOARD_SQUARE_SIZE*2, checkerY + BOARD_SQUARE_SIZE*2);
                        possibleJumps.emplace_back(checkerX - BOARD_SQUARE_SIZE*2, checkerY + BOARD_SQUARE_SIZE*2);
                        jumpPositions.emplace_back(checkerX + BOARD_SQUARE_SIZE, checkerY + BOARD_SQUARE_SIZE);
                        jumpPositions.emplace_back(checkerX - BOARD_SQUARE_SIZE, checkerY + BOARD_SQUARE_SIZE);
                    }

                    bool hasJump = false;
                    for(int j = 0; j < jumpPositions.size(); j++) {
                        if(jumpPositions[j].x < 275 || jumpPositions[j].x > 525 || possibleJumps[j].x < 275 || possibleJumps[j].x > 525) {
                            continue;
                        }
                        if(jumpPositions[j].y < 125 || jumpPositions[j].y > 475 || possibleJumps[j].y < 125 || possibleJumps[j].y > 475) {
                            continue;
                        }
                        if(checkerAt(jumpPositions[j].x, jumpPositions[j].y, currentPlayer) && !checkerAt(possibleJumps[j].x, possibleJumps[j].y, NONE)) {
                            hasJump = true;
                            break;
                        }
                    }
                    if(hasJump) {
                        isJumpingTurn = true;
                        jumpingChecker = currentChecker;
                        currentChecker = nullptr;
                        free(checkerToRemove);
                        free(prevPos);
                        free(currentPos);
                        return;
                    }
                }
            }
            free(checkerToRemove);
            free(prevPos);
            free(currentPos);
        }else {
            currentChecker->setPosition(lastCheckerPosition.x, lastCheckerPosition.y);
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
    Checker* clickedChecker = nullptr;
    for(Checker* checker : checkers) {
        if(checkBounds(posX, posY, checker) && checker->getPlayer() == currentPlayer) {
            clickedChecker = checker;
            break;
        }
    }
    if(clickedChecker != nullptr) {
        currentChecker = clickedChecker;
        lastCheckerPosition = clickedChecker->getPosition();
        clickedChecker->setPosition(posX, posY);
    }
}

void Game::processMouseMove(const sf::Event& e) const {
    int posX = e.mouseMove.x;
    int posY = e.mouseMove.y;
    if(currentChecker != nullptr) {
        currentChecker->setPosition(posX, posY);
    }
}

void Game::onTurnChange() {
    currentPlayer = getOpposite(currentPlayer);
    movesSinceLastCapture++;
    int** boardCopy = static_cast<int **>(malloc(sizeof(int*) * BOARD_HEIGHT));
    for (int i = 0; i < BOARD_HEIGHT; ++i) {
        boardCopy[i] = static_cast<int *>(malloc(sizeof(int) * BOARD_WIDTH));
    }
    for(int i = 0; i < BOARD_WIDTH; i++) {
        for(int j = 0; j < BOARD_HEIGHT; j++) {
            boardCopy[i][j] = board[i][j];
        }
    }
    statesSinceLastCapture.push_back(boardCopy);
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
    initializeBoard();
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
        screenManager->drawScreen(checkers, currentPlayer, gameState);

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
    for (int** b : statesSinceLastCapture) {
        // Free each row
        for (int i = 0; i < BOARD_HEIGHT; ++i) {
            free(b[i]);
        }
        // Free the array of pointers
        free(b);
    }
    statesSinceLastCapture.clear();
}

void Game::checkGameState() {
    if(movesSinceLastCapture >= 100){
        gameState = DRAW;
        return;
    }
    std::vector<int**> statesCopy = statesSinceLastCapture;
    if (!statesCopy.empty()) {
        int** currentBoard = statesCopy.back();
        statesCopy.pop_back();
        while(!statesCopy.empty()){
            int sameBoardCount = 0;
            for(int** b : statesSinceLastCapture){
                bool isSame = true;
                for(int i = 0; i < BOARD_HEIGHT; i++){
                    for(int j = 0; j < BOARD_WIDTH; j++){
                        std::cout << currentBoard[i][j] << " " << b[i][j] << std::endl;
                        if(currentBoard[i][j] != b[i][j]){
                            isSame = false;
                            break;
                        }
                        if(!isSame){
                            break;
                        }
                    }
                }
                if(isSame){
                    sameBoardCount++;
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
    if(checkWin(RED)){
        gameState = RED_WIN;
        return;
    }
    if(checkWin(BLACK)){
        gameState = BLACK_WIN;
        return;
    }
}

bool Game::checkWin(Player player) const {
    int redCount = 0;
    int blackCount = 0;
    for(int i = 0; i < BOARD_HEIGHT; i++){
        for(int j = 0; j < BOARD_WIDTH; j++){
            if(board[i][j] == BLACK){
                blackCount++;
            }else if(board[i][j] == RED){
                redCount++;
            }
        }
    }
    if(player == RED && blackCount == 0){
        return true;
    }
    if(player == BLACK && redCount == 0){
        return true;
    }
    return false;
}

int main() {
    Game* g = new Game();
    g->run();
    delete g;
    return 0;
}