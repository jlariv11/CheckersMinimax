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
    // Move is invalid if there is a checker at the current position
    if(this->board.getCheckerAt(currentChecker->getPosition(), NONE) != nullptr) {
        return -1;
    }
    // Move is invalid if the checker moved is not the same as the one used to make a jump
    // Double jumping
    if(isJumpingTurn && currentChecker != jumpingChecker) {
        return -1;
    }
    // Create a temporary checker with the current checker's position before being picked up
    std::shared_ptr<Checker> temp = std::make_shared<Checker>(lastCheckerPosition, currentChecker->getPlayer(), -1);
    if(currentChecker->isKing()) {
        temp->setKing();
    }
    // Generate all possible moves that could be made from that position
    std::vector<sf::Vector2i> moves = getMoves(this->board, temp, false);
    sf::Vector2i pos = this->board.getClosestPosition(currentChecker->getPosition());
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
    // Player clicks while carrying a checker
    if(currentChecker != nullptr) {
        int validMove = checkValidMove();
        // If the player makes an invalid move, put the checker back where it started
        if(validMove == -1) {
            this->board.moveChecker(lastCheckerPosition, lastCheckerPosition, false);
            currentChecker = nullptr;
            return;
        }
        // Otherwise, move the checker to the clicked position
        this->board.moveChecker(lastCheckerPosition, currentChecker->getPosition(), validMove);
        onCheckerMove(lastCheckerPosition, currentChecker->getPosition(), currentChecker);
        // If that move was a jump, check if there is another jump available
        if(validMove == 1) {
            onPieceCapture();
            // If there is another jump available, allow the player to make the jump (double jump)
            // Skips turn change code
            if(hasMoves(this->board, currentChecker, true)) {
                isJumpingTurn = true;
                jumpingChecker = currentChecker;
                currentChecker = nullptr;
                return;
            }
        }
        // Switch to next player's turn
        isJumpingTurn = false;
        jumpingChecker = nullptr;
        onTurnChange();
        currentChecker = nullptr;
        return;
    }
    // Checker Pickup click
    int posX = e.mouseButton.x;
    int posY = e.mouseButton.y;
    std::shared_ptr<Checker> clickedChecker = this->board.getCheckerAt({posX, posY}, NONE);
    // Make sure the spot clicked has a checker, and the checker is the same color as the current player
    if(clickedChecker != nullptr) {
        if(clickedChecker->getPlayer() != currentPlayer) {
            return;
        }
        // Update current checker to allow it to follow the mouse
        currentChecker = clickedChecker;
        lastCheckerPosition = clickedChecker->getPosition();
        clickedChecker->setPosition({posX, posY});
    }
}

void Game::processMouseMove(const sf::Event& e) const {
    // Update the current checker's position to the mouse position when it moves
    if(currentChecker != nullptr) {
        currentChecker->setPosition({e.mouseMove.x, e.mouseMove.y});
    }
}

void Game::onTurnChange() {
    // Update the current turn, and moves and states since last capture
    currentPlayer = getOpposite(currentPlayer);
    movesSinceLastCapture++;
    statesSinceLastCapture.push_back(this->board);
    // Do the AI's turn
    if(currentPlayer == BLACK) {
        aiTurn();
    }
}

void Game::onCheckerMove(sf::Vector2i from, sf::Vector2i to, std::shared_ptr<Checker> checker) {
    // Check weather the movement results in a king
    if(checker->getPlayer() == RED && to.y == 475 && !checker->isKing()) {
        checker->setKing();
    }
    if(checker->getPlayer() == BLACK && to.y == 125 && !checker->isKing()) {
        checker->setKing();
    }
    // Update the game state
    gameState = checkGameState(this->board);
}

GameState Game::hasWinner(Board board) {
    // If a player runs out of checkers, it is a win for the other player
    if(board.getBlackCheckers().size() == 0) {
        return RED_WIN;
    }
    if(board.getRedCheckers().size() == 0) {
        return BLACK_WIN;
    }
    return IN_PROGRESS;
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
        screenManager->drawScreen(this->board, currentPlayer, gameState);

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
    // Clear the data for moves and states since last capture
    movesSinceLastCapture = 0;
    statesSinceLastCapture.clear();
}

// Rules based on https://cardgames.io/checkers/
GameState Game::checkGameState(Board board) {
    // Game is a draw if there have been 100 moves (50 each) between the last checker capture
    if(movesSinceLastCapture >= 100){
        return DRAW;
    }
    // If the same board state has shown up at least 3 times since the last capture, the game is a draw
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
                return DRAW;
            }
        }
    }
    // If a player runs out of possible moves, the other player is the winner
    bool canMove = false;
    for(int i = 0; i < BOARD_WIDTH; i++) {
        for(int j = 0; j < BOARD_HEIGHT; j++) {
            std::shared_ptr<Checker> c = board.getCheckerAtArray({i, j});
            if(c != nullptr) {
                if(c->getPlayer() != getOpposite(currentPlayer)) {
                    continue;
                }
                if(hasMoves(board, c, false)) {
                    canMove = true;
                    break;
                }
            }
        }
        if(canMove) {
            break;
        }
    }
    if(!canMove) {
        return getOpposite(currentPlayer) == RED ? BLACK_WIN : RED_WIN;
    }
    // Check for a winner
    return hasWinner(board);
}


bool Game::hasMoves(Board board, std::shared_ptr<Checker> checker, bool onlyJump) {
    return !getMoves(board, checker, onlyJump).empty();
}


std::vector<sf::Vector2i> Game::getMoves(Board board, std::shared_ptr<Checker> checker, bool onlyJump) {
    int checkerX = checker->getPosition().x;
    int checkerY = checker->getPosition().y;
    std::vector<sf::Vector2i> movePositions{};
    std::vector<sf::Vector2i> jumpPositions{};
    std::vector<sf::Vector2i> moves{};
    // Generate all possible positions for a BLACK, RED or KING to move or jump
    if(checker->isKing()) {
        jumpPositions.emplace_back(checkerX + BOARD_SQUARE_SIZE*2, checkerY + BOARD_SQUARE_SIZE*2);
        jumpPositions.emplace_back(checkerX - BOARD_SQUARE_SIZE*2, checkerY + BOARD_SQUARE_SIZE*2);
        jumpPositions.emplace_back(checkerX + BOARD_SQUARE_SIZE*2, checkerY - BOARD_SQUARE_SIZE*2);
        jumpPositions.emplace_back(checkerX - BOARD_SQUARE_SIZE*2, checkerY - BOARD_SQUARE_SIZE*2);

        movePositions.emplace_back(checkerX + BOARD_SQUARE_SIZE, checkerY + BOARD_SQUARE_SIZE);
        movePositions.emplace_back(checkerX - BOARD_SQUARE_SIZE, checkerY + BOARD_SQUARE_SIZE);
        movePositions.emplace_back(checkerX + BOARD_SQUARE_SIZE, checkerY - BOARD_SQUARE_SIZE);
        movePositions.emplace_back(checkerX - BOARD_SQUARE_SIZE, checkerY - BOARD_SQUARE_SIZE);
    }else if(checker->getPlayer() == BLACK) {
        jumpPositions.emplace_back(checkerX + BOARD_SQUARE_SIZE*2, checkerY - BOARD_SQUARE_SIZE*2);
        jumpPositions.emplace_back(checkerX - BOARD_SQUARE_SIZE*2, checkerY - BOARD_SQUARE_SIZE*2);
        movePositions.emplace_back(checkerX + BOARD_SQUARE_SIZE, checkerY - BOARD_SQUARE_SIZE);
        movePositions.emplace_back(checkerX - BOARD_SQUARE_SIZE, checkerY - BOARD_SQUARE_SIZE);
    }else if(checker->getPlayer() == RED) {
        jumpPositions.emplace_back(checkerX + BOARD_SQUARE_SIZE*2, checkerY + BOARD_SQUARE_SIZE*2);
        jumpPositions.emplace_back(checkerX - BOARD_SQUARE_SIZE*2, checkerY + BOARD_SQUARE_SIZE*2);
        movePositions.emplace_back(checkerX + BOARD_SQUARE_SIZE, checkerY + BOARD_SQUARE_SIZE);
        movePositions.emplace_back(checkerX - BOARD_SQUARE_SIZE, checkerY + BOARD_SQUARE_SIZE);
    }

    // Check for jumps
    for(int i = 0; i < jumpPositions.size(); i++) {
        // Check if the move is out of bounds
        if(jumpPositions[i].x < 200 || jumpPositions[i].x > 600 || movePositions[i].x < 200 || movePositions[i].x > 600) {
            continue;
        }
        if(jumpPositions[i].y < 100 || jumpPositions[i].y > 500 || movePositions[i].y < 100 || movePositions[i].y > 500) {
            continue;
        }
        // Check if the move is valid
        // The space where the checker jumps to(jumpPositions) must be clear
        // AND the space the checker is jumping over(movePositions) must have a checker of the opposite color
        if(board.getCheckerAt(jumpPositions[i], NONE) == nullptr && board.getCheckerAt(movePositions[i], checker->getPlayer()) != nullptr) {
            moves.push_back(jumpPositions[i]);
        }
    }
    // Check for regular moves
    if(!onlyJump) {
        for(int i = 0; i < movePositions.size(); i++) {
            // Check if the move is out of bounds
            if(movePositions[i].x < 200 || movePositions[i].x > 600) {
                continue;
            }
            if(movePositions[i].y < 100 || movePositions[i].y > 500) {
                continue;
            }
            // Check if the move is valid
            // The space the checker is moving to must be clear
            if(board.getCheckerAt(movePositions[i], NONE) == nullptr) {
                moves.push_back(movePositions[i]);
            }
        }
    }

    return moves;
}
void Game::aiTurn() {
    std::shared_ptr<Checker> bestChecker = nullptr;
    sf::Vector2i bestMove;
    int bestScore = -9999;

    // Loop through every black checker and generate its possible moves
    for(std::shared_ptr<Checker> c : this->board.getBlackCheckers()) {
        std::vector<sf::Vector2i> possibleMoves = getMoves(this->board, c, false);
        // Loop through each possible move for the checker
        for(sf::Vector2i m : possibleMoves) {
            sf::Vector2i lastPos = c->getPosition();
            bool isJumpMove = (m.x - BOARD_SQUARE_SIZE*2) == lastPos.x || (m.x + BOARD_SQUARE_SIZE*2) == lastPos.x;
            // Create a board copy for each possible move
            Board b = this->board;
            // Check if the move performed was a jump
            // Move the checker on the board copy
            b.moveChecker(lastPos, m, isJumpMove);
            // Perform minimax on this board state (is now minimizing (Player's move))
            int score = minimax(b, 4, false);
            // Compare the score to the current best score (AI is maximizing so the highest score)
            // Update the score, move to make, and the checker to move
            if(score > bestScore) {
                bestScore = score;
                bestMove = m;
                bestChecker = c;
            }
        }
    }
    // After checking every possible move for each black checker, make the best move on the real board
    if(bestChecker != nullptr) {
        sf::Vector2i lastPos = bestChecker->getPosition();
        bool isJumpMove = (bestMove.x - BOARD_SQUARE_SIZE*2) == lastPos.x || (bestMove.x + BOARD_SQUARE_SIZE*2) == lastPos.x;
        this->board.moveChecker(lastPos, bestMove, isJumpMove);
        onCheckerMove(lastPos, bestMove, bestChecker);
        if(isJumpMove) {
            onPieceCapture();
        }
        onTurnChange();
    }
}


// AI is maximizing
// Player is minimizing
int Game::minimax(Board board, int depth, bool isMaximizing) {
    /*
     * Score:
     * +10 AI capture
     * -10 Player capture
     * +5 AI king
     * -5 Player king
     */
    // Check the current board state for a winner
    GameState state = checkGameState(board);
    if(state != IN_PROGRESS) {
        if(state == DRAW) {
            return isMaximizing ? -10 : 10;
        }
        // If we are maximizing (AI BLACK) a red win is the worst outcome
        // If we are minimizing (Player RED) a black win is the worst outcome
        if(isMaximizing) {
            return state == RED_WIN ? -9999 : (9999 - depth);
        }else {
            return state == BLACK_WIN ? 9999 : (-9999 + depth);
        }
    }
    if(depth <= 0) {
        if(isMaximizing) {
            return board.getBlackCheckers().size();
        }else {
            return -board.getRedCheckers().size();
        }
    }

    if(isMaximizing) {
        int bestScore = -9999;
        // Loop through every black checker and generate its possible moves
        for(std::shared_ptr<Checker> c : board.getBlackCheckers()) {
            std::vector<sf::Vector2i> possibleMoves = getMoves(board, c, false);
            // Loop through each possible move for the checker
            for(sf::Vector2i m : possibleMoves) {
                int moveScore = 0;
                sf::Vector2i lastPos = c->getPosition();
                // Create a board copy for each possible move
                Board b = board;
                // Check if the move performed was a jump
                bool isJumpMove = (m.x - BOARD_SQUARE_SIZE*2) == lastPos.x || (m.x + BOARD_SQUARE_SIZE*2) == lastPos.x;
                // If the move captures a checker, give it +10 score
                if(isJumpMove) {
                    moveScore += 10;
                }
                // If the move creates a king, give it +5 score
                if(!c->isKing() && m.y == 125) {
                    moveScore += 5;
                }
                // Move the checker on the board copy
                b.moveChecker(lastPos, m, isJumpMove);
                // Perform minimax on this board state (is now minimizing (Player's move))
                // If the AI jumps and has an available move, it gets another turn
                int score = minimax(b, depth-1, isJumpMove && hasMoves(b, c, true));
                bestScore = std::max(bestScore, score + moveScore);
            }
        }
        return bestScore;
    }else {
        int bestScore = 9999;
        // Loop through every red checker and generate its possible moves
        for(std::shared_ptr<Checker> c : board.getRedCheckers()) {
            std::vector<sf::Vector2i> possibleMoves = getMoves(board, c, false);
            // Loop through each possible move for the checker
            for(sf::Vector2i m : possibleMoves) {
                int moveScore = 0;
                sf::Vector2i lastPos = c->getPosition();
                // Create a board copy for each possible move
                Board b = board;
                // Check if the move performed was a jump
                bool isJumpMove = (m.x - BOARD_SQUARE_SIZE*2) == lastPos.x || (m.x + BOARD_SQUARE_SIZE*2) == lastPos.x;
                // If the move captures a checker, give it -10 score
                if(isJumpMove) {
                    moveScore -= 10;
                }
                // If the move creates a king, give it -5 score
                if(!c->isKing() && m.y == 425) {
                    moveScore -= 5;
                }
                // Move the checker on the board copy
                b.moveChecker(lastPos, m, isJumpMove);
                // Perform minimax on this board state (is now maximizing (AI's move))
                int score = minimax(b, depth-1, !(isJumpMove && hasMoves(b, c, true)));
                bestScore = std::min(bestScore, score + moveScore);
            }
        }
        return bestScore;
    }
}


int main() {
    Game g;
    g.run();
    return 0;
}