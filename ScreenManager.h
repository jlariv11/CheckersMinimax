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
    void drawScreen(const std::vector<Checker*>& checkers, Player currentPlayer, GameState gameState) const;
    void start();
    sf::RenderWindow* getWindow() const;
    ScreenManager(const ScreenManager&) = delete;
    ScreenManager& operator=(const ScreenManager&) = delete;

    private:
    ScreenManager();
    ~ScreenManager();
    sf::RenderWindow* window;
    sf::Font font;
    void drawBackground() const;
    void drawInfo(Player currentPlayer, GameState gameState) const;
    static void drawCheckers(const std::vector<Checker*> &checkers);
};

#endif //SCREENMANAGER_H
