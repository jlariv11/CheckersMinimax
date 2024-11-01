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
    std::shared_ptr<Checker> currentChecker;
    sf::Vector2i lastCheckerPosition;
    bool isJumpingTurn;
    std::shared_ptr<Checker> jumpingChecker;
    int movesSinceLastCapture;
    void onPieceCapture();
    void onTurnChange();
    void onCheckerMove(sf::Vector2i from, sf::Vector2i to, std::shared_ptr<Checker>);
    void checkGameState();
    void processMouseClick(const sf::Event& e);
    void processMouseMove(const sf::Event& e) const;
    static bool checkBounds(int mouseX, int mouseY, std::shared_ptr<Checker>);
    bool hasMoves(std::shared_ptr<Checker>, bool onlyJump);
    std::vector<sf::Vector2i> getMoves(std::shared_ptr<Checker> checker, bool onlyJump);
    int checkValidMove();
    int minimax(Board board, int depth, int score, bool isMaximizing);
    void aiTurn();

};


#endif //GAME_H
