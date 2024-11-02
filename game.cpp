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
    std::shared_ptr<Checker> temp = std::make_shared<Checker>(lastCheckerPosition, currentChecker->getPlayer(), -1);
    if(currentChecker->isKing()) {
        temp->setKing();
    }
    // Generate all possible moves that could be made from that position
    std::vector<sf::Vector2i> moves = getMoves(temp, false);
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
    //std::cout << e.mouseButton.x << ", " << e.mouseButton.y << std::endl;
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
    std::shared_ptr<Checker> clickedChecker = board.getCheckerAt({posX, posY}, NONE);
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
        aiTurn();
    }
}

void Game::onCheckerMove(sf::Vector2i from, sf::Vector2i to, std::shared_ptr<Checker> checker) {
    if(checker->getPlayer() == RED && to.y == 475 && !checker->isKing()) {
        checker->setKing();
    }
    if(checker->getPlayer() == BLACK && to.y == 125 && !checker->isKing()) {
        checker->setKing();
    }
    checkGameState();
}


bool Game::checkBounds(int mouseX, int mouseY, std::shared_ptr<Checker> checker) {
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
            std::shared_ptr<Checker> c = board.getCheckerAtArray({i, j});
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
        if(canMove) {
            break;
        }
    }
    if(!canMove) {
        gameState = getOpposite(currentPlayer) == RED ? BLACK_WIN : RED_WIN;
    }
    gameState = board.hasWinner();
}


bool Game::hasMoves(std::shared_ptr<Checker> checker, bool onlyJump) {
    return !getMoves(checker, onlyJump).empty();
}


std::vector<sf::Vector2i> Game::getMoves(std::shared_ptr<Checker> checker, bool onlyJump) {
    int checkerX = checker->getPosition().x;
    int checkerY = checker->getPosition().y;
    std::vector<sf::Vector2i> movePositions{};
    std::vector<sf::Vector2i> jumpPositions{};
    std::vector<sf::Vector2i> moves{};

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
    int bestScore = -1;

    // Loop through every black checker and generate its possible moves
    for(std::shared_ptr<Checker> c : board.getBlackCheckers()) {
        std::vector<sf::Vector2i> possibleMoves = getMoves(c, false);
        //std::cout << "Possible moves for checker id: " << c->getID() << " at position: " << c->getPosition().x << ", " << c->getPosition().y << std::endl;
        // Loop through each possible move for the checker
        for(sf::Vector2i m : possibleMoves) {
            sf::Vector2i lastPos = c->getPosition();
            bool isJumpMove = (m.x - BOARD_SQUARE_SIZE*2) == lastPos.x || (m.x + BOARD_SQUARE_SIZE*2) == lastPos.x;
            //std::cout << m.x << ", " << m.y << (isJumpMove ? " and its a jump" : "") << std::endl;
            // Create a board copy for each possible move
            Board b = board;
            // Check if the move performed was a jump
            // Move the checker on the board copy
            b.moveChecker(lastPos, m, isJumpMove);
            // Perform minimax on this board state (is now minimizing (Player's move))
            int score = minimax(b, 5, false);
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
int Game::minimax(Board board, int depth, bool isMaximizing) {
    /*
     * Score:
     * +10 AI capture
     * -10 Player capture
     * +5 AI king
     * -5 Player king
     */
    //TODO: or draws
    // Check the current board state for a winner
    if(board.hasWinner() != IN_PROGRESS) {
        // If we are maximizing (AI BLACK) a red win is worst outcome
        // If we are minimizing (Player RED) a black win is worst outcome
        if(isMaximizing) {
            return board.hasWinner() == RED_WIN ? (-INFINITY) : (1000 - depth);
        }else {
            return board.hasWinner() == BLACK_WIN ? (INFINITY) : (-1000 + depth);
        }
    }
    if(depth <= 0) {
        if(isMaximizing) {
            return (board.getBlackCheckers().size() - board.getRedCheckers().size()) * 10;
        }else {
            std::cout << (board.getRedCheckers().size() - board.getBlackCheckers().size()) * 10 << std::endl;
            return (board.getRedCheckers().size() - board.getBlackCheckers().size()) * 10;
        }
    }

    if(isMaximizing) {
        int bestScore = -INFINITY;
        // Loop through every black checker and generate its possible moves
        for(std::shared_ptr<Checker> c : board.getBlackCheckers()) {
            std::vector<sf::Vector2i> possibleMoves = getMoves(c, false);
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
                int score = minimax(b, depth-1, false);
                bestScore = std::max(bestScore, score + moveScore);
            }
        }
        return bestScore;
    }else {
        int bestScore = INFINITY;
        // Loop through every red checker and generate its possible moves
        for(std::shared_ptr<Checker> c : board.getRedCheckers()) {
            std::vector<sf::Vector2i> possibleMoves = getMoves(c, false);
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
                int score = minimax(b, depth-1, true);
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