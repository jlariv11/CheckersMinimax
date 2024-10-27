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
    std::vector<sf::Vector2i> boardPositions;
    std::vector<Checker*> checkers;
    Player currentPlayer;
    GameState gameState;
    std::vector<int**> statesSinceLastCapture;
    Checker* currentChecker;
    sf::Vector2i lastCheckerPosition;
    bool isJumpingTurn;
    Checker* jumpingChecker;
    int movesSinceLastCapture;
    void initializeBoard();
    void onPieceCapture();
    void onTurnChange();
    void onCheckerMove(sf::Vector2i from, sf::Vector2i to, Checker* checker);
    void checkGameState();
    void processMouseClick(const sf::Event& e);
    void processMouseMove(const sf::Event& e) const;
    static bool checkBounds(int mouseX, int mouseY, Checker* checker);
    int checkValidMove() const;
    static int* worldToBoard(sf::Vector2i coordinate);
    bool checkerAt(int x, int y, Player color) const;
    static sf::Vector2i getClosestPosition(Checker* checker);

    static int round(int num);
    bool checkWin(Player player) const;

};


#endif //GAME_H
