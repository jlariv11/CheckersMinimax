//
// Created by Jake Lariviere on 10/20/24.
//

#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H
#include "Checker.h"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/RenderWindow.hpp"


class ScreenManager {
    public:
    static ScreenManager* getInstance();
    void drawScreen(std::vector<Checker*> checkers, Player currentPlayer, GameState gameState);
    void start();
    sf::RenderWindow* getWindow();
    ScreenManager(const ScreenManager&) = delete;
    ScreenManager& operator=(const ScreenManager&) = delete;

    private:
    ScreenManager();
    ~ScreenManager();
    sf::RenderWindow* window;
    sf::Font font;
    void drawBackground();
    void drawInfo(Player currentPlayer, GameState gameState);
    void drawCheckers(std::vector<Checker*> checkers);
};

#endif //SCREENMANAGER_H
