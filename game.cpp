#include <iostream>
#include "game.h"

#include <iomanip>
#include <sfml/Window.hpp>

#include "Checker.h"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Text.hpp"
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
    boardPositions = std::vector<sf::Vector2f>(32);
    checkers = std::vector<Checker*>(24);
}

Game::~Game() {
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        free(board[i]);
    }
    free(board);
}

void Game::initalizeBoard() {
    bool red = true;
    for(int i = 0; i < BOARD_HEIGHT; i++){
        for(int j = 0; j < BOARD_WIDTH; j++){
            board[i][j] = NONE;
            red = !red;
        }
        red = !red;
    }

    for(int i = 0; i < BOARD_HEIGHT; i++){
        if(i == 3 || i == 4){
            continue;
        }
        for(int j = 0; j < BOARD_WIDTH; j++){
            board[i][j] = i > 3 ? RED : BLACK;
        }
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
        boardPositions[i] = sf::Vector2f(posX, posY);
        std::cout << posX << " " << posY << std::endl;
        posX += BOARD_SQUARE_SIZE * 2;
    }

    for(int i = 0; i < 12; i++) {
        checkers[i] = new Checker(boardPositions[i].x, boardPositions[i].y, RED, i);
    }
    for(int i = 20; i < 32; i++) {
        checkers[i-8] = new Checker(boardPositions[i].x, boardPositions[i].y, BLACK, i);
    }
}
int Game::round(int num) {
    return ((num + 12) / 25) * 25;
}

sf::Vector2f Game::getClosestPosition(Checker* checker) {
    int x = round(checker->getPosition().x);
    int y = round(checker->getPosition().y);
    if(x % 10 == 0) {
        if(checker->getPosition().x < x) {
            x -= 25;
        }else {
            x += 25;
        }
    }
    if(y % 10 == 0) {
        if(checker->getPosition().y < y) {
            y -= 25;
        }else {
            y += 25;
        }
    }
    return sf::Vector2f(x, y);
}

Checker* Game::findCheckerAt(int x, int y, int ignoreID) {
    for(int i = 0; i < checkers.size(); i++) {
        if(checkers[i]->getID() == ignoreID) {
            continue;
        }
        if(x <= checkers[i]->getPosition().x + CHECKER_RADIUS && x >= checkers[i]->getPosition().x - CHECKER_RADIUS) {
            if(y <= checkers[i]->getPosition().y + CHECKER_RADIUS && y >= checkers[i]->getPosition().y - CHECKER_RADIUS) {
                return checkers[i];
            }
        }
    }
    return nullptr;
}


bool Game::checkValidMove() {
    if(findCheckerAt(currentChecker->getPosition().x, currentChecker->getPosition().y, currentChecker->getID()) != nullptr) {
        return false;
    }
    int deltaX = lastCheckerPosition.x - currentChecker->getPosition().x;
    int deltaY = lastCheckerPosition.y - currentChecker->getPosition().y;
    float angle = atan2(deltaY, deltaX);
    double distance = sqrt(deltaX * deltaX + deltaY * deltaY);
    // Regular move: between 45 and 95
    // Jump move: between 120 and 170
    if(distance > 95) {
        int xHalf = lastCheckerPosition.x + deltaX;
        int yHalf = lastCheckerPosition.y + deltaY;
        //std::cout << xHalf << std::endl;
        //std::cout << yHalf << std::endl;
        //std::cout << (findCheckerAt(xHalf, yHalf) != nullptr) << std::endl;
    }
    return (angle < -0.7 && angle > -2.5) && (distance > 45 && distance < 95);
}


void Game::processMouseClick(sf::Event& e) {
    //std::cout << "Mouse" << std::endl;
    //std::cout << e.mouseButton.x << std::endl;
    //std::cout << e.mouseButton.y << std::endl;
    if(currentChecker != nullptr) {
        if(checkValidMove()) {
            sf::Vector2f pos = getClosestPosition(currentChecker);
            currentChecker->setPosition(pos.x, pos.y);
            currentChecker = nullptr;
        }else {
            currentChecker->setPosition(lastCheckerPosition.x, lastCheckerPosition.y);
        }
        currentChecker = nullptr;
        return;
    }
    int posX = e.mouseButton.x;
    int posY = e.mouseButton.y;
    Checker* clickedChecker = nullptr;
    for(int i = 0; i < checkers.size(); i++) {
        if(checkBounds(posX, posY, checkers[i]) && checkers[i]->getPlayer() == currentPlayer) {
            clickedChecker = checkers[i];
            break;
        }
    }
    if(clickedChecker != nullptr) {
        currentChecker = clickedChecker;
        lastCheckerPosition = clickedChecker->getPosition();
        clickedChecker->setPosition(posX, posY);
    }
}

void Game::processMouseMove(sf::Event& e) {
    int posX = e.mouseMove.x;
    int posY = e.mouseMove.y;
    if(currentChecker != nullptr) {
        currentChecker->setPosition(posX, posY);
    }
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
    initalizeBoard();
    ScreenManager *screenManager = ScreenManager::getInstance();
    screenManager->start();
    long startTime;
    long currentTime;
    long adjustTime;
    while(screenManager->getWindow()->isOpen() && gameState == IN_PROGRESS) {
        startTime = sf::Clock().getElapsedTime().asMilliseconds();

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
        // Process Input

        // Player Turn these need to happen when a turn is confirmed
        movesSinceLastCapture++;
        statesSinceLastCapture.push_back(board);
        // Check Game State
        //checkGameState();
        // AI turn
        movesSinceLastCapture++;
        statesSinceLastCapture.push_back(board);
        // Check Game State
        //checkGameState();


        // Render
        screenManager->drawScreen(checkers);

        currentTime = sf::Clock().getElapsedTime().asMilliseconds();
        long deltaTime = currentTime - startTime;
        long intendedSleepTime = 33 - deltaTime - adjustTime;
        if(intendedSleepTime < 0) {
            intendedSleepTime = 0;
        }
        sf::sleep(sf::milliseconds(intendedSleepTime));
        adjustTime = sf::Clock().getElapsedTime().asMilliseconds() - intendedSleepTime;
        if(adjustTime < 0) {
            adjustTime = 0;
        }


        sf::Text text;
        //text.setFont(font);
        switch (gameState){
            case RED_WIN:
                text.setString("Red Win!");
            break;
            case BLACK_WIN:
                text.setString("Black Win!");
            break;
            case DRAW:
                text.setString("Draw");
            break;
            default:
                text.setString("Game Over!");
            break;
        }
        text.setCharacterSize(40);
        text.setColor(sf::Color::White);
        text.setPosition(sf::Vector2f(360, 260));
        //window.clear();
        //window.draw(text);
        //window.display();
    }
}

void Game::onPieceCapture() {
    movesSinceLastCapture = 0;
    statesSinceLastCapture.clear();
}

void Game::checkGameState() {
    if(movesSinceLastCapture >= 100){
        gameState = DRAW;
        return;
    }
    std::vector<int**> statesCopy = statesSinceLastCapture;
    int** currentBoard = statesCopy.back();
    statesCopy.pop_back();
    int sameBoardCount = 0;
    while(!statesCopy.empty()){
        for(int** b : statesSinceLastCapture){
            bool isSame = true;
            for(int i = 0; i < BOARD_HEIGHT; i++){
                for(int j = 0; j < BOARD_WIDTH; j++){
                    if(!currentBoard[i][j] == b[i][j]){
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

    if(checkWin(RED)){
        gameState = RED_WIN;
        return;
    }
    if(checkWin(BLACK)){
        gameState = BLACK_WIN;
        return;
    }
}

bool Game::checkWin(Player player) {
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