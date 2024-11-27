//
// Created by Jake Lariviere on 11/24/24.
//

#ifndef GAME_H
#define GAME_H
#include <vector>

#include "Board.h"
#include "WorldChecker.h"
#include "SFML/Window/Event.hpp"

class Game {

public:
    Game();
    void run();
private:
    Board board;
    WorldChecker* clickedChecker;
    sf::Vector2i clickedOriginalPos;
    std::vector<WorldChecker*> worldCheckers;
    sf::Vector2i worldToBoard(sf::Vector2i world);
    sf::Vector2i boardToWorld(sf::Vector2i board);
    sf::Vector2i normalizeWorldPos(sf::Vector2i world);
    void onMouseClick(sf::Event &event);
    void onMouseMove(sf::Event &event);
};


#endif //GAME_H
