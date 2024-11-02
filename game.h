//
// Created by Jake Lariviere on 10/14/24.
//
#ifndef GAME_H
#define GAME_H
#include <vector>

#include "Board.h"
#include "GameProps.h"

#include "SFML/Graphics/RenderWindow.hpp"

class Game {

/*
 * The game is responsible for:
 * the game loop
 * identifying the game state
 * controlling and verifying player moves
 * running Minimax for the AI's turn
 */

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
    GameState checkGameState(Board board);
    void processMouseClick(const sf::Event& e);
    void processMouseMove(const sf::Event& e) const;
    bool hasMoves(Board board, std::shared_ptr<Checker>, bool onlyJump);
    std::vector<sf::Vector2i> getMoves(Board board, std::shared_ptr<Checker> checker, bool onlyJump);
    int checkValidMove();
    int minimax(Board board, int depth, bool isMaximizing);
    void aiTurn();
    GameState hasWinner(Board board);

};


#endif //GAME_H
