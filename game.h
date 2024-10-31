//
// Created by Jake Lariviere on 10/14/24.
//
#ifndef GAME_H
#define GAME_H
#include <vector>

#include "Board.h"
#include "GameProps.h"

#include "SFML/Graphics/RenderWindow.hpp"

class Checker;

class Game {
public:
    Game();
    ~Game();
    void run();

private:
    Board board;
    Player currentPlayer;
    GameState gameState;
    std::vector<Board> statesSinceLastCapture;
    Checker* currentChecker;
    sf::Vector2i lastCheckerPosition;
    bool isJumpingTurn;
    Checker* jumpingChecker;
    int movesSinceLastCapture;
    void onPieceCapture();
    void onTurnChange();
    void onCheckerMove(sf::Vector2i from, sf::Vector2i to, Checker* checker);
    void checkGameState();
    void processMouseClick(const sf::Event& e);
    void processMouseMove(const sf::Event& e) const;
    static bool checkBounds(int mouseX, int mouseY, Checker* checker);
    bool hasMoves(Checker* checker, bool onlyJump);
    std::vector<sf::Vector2i> getMoves(Checker* checker, bool onlyJump);
    int checkValidMove();
    int minimax(Board board, int depth, int score, bool isMaximizing);
    void aiTurn();

};


#endif //GAME_H
