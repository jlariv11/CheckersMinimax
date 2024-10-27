//
// Created by Jake Lariviere on 10/14/24.
//
#ifndef GAME_H
#define GAME_H
#include <vector>
#include "GameProps.h"

#include "SFML/Graphics/RenderWindow.hpp"

class Checker;

class Game {
public:
    Game();
    ~Game();
    void run();

private:
    int** board;
    std::vector<sf::Vector2f> boardPositions;
    std::vector<Checker*> checkers;
    Player currentPlayer;
    GameState gameState;
    std::vector<int**> statesSinceLastCapture;
    Checker* currentChecker;
    sf::Vector2f lastCheckerPosition;
    int movesSinceLastCapture;
    void initalizeBoard();
    void onPieceCapture();
    void checkGameState();
    void processMouseClick(sf::Event& e);
    void processMouseMove(sf::Event& e);
    bool checkBounds(int mouseX, int mouseY, Checker* checker);
    int checkValidMove();
    int* worldToBoard(sf::Vector2f coordinate);
    bool checkerAt(int x, int y, Player color);
    sf::Vector2f getClosestPosition(Checker* checker);
    int round(int num);
    bool checkWin(Player player);

};


#endif //GAME_H
