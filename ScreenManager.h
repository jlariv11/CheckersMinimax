//
// Created by Jake Lariviere on 11/24/24.
//

#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include "Board.h"
#include "WorldChecker.h"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/RenderWindow.hpp"

class ScreenManager {
public:
    static ScreenManager* getInstance();
    void drawScreen(std::vector<WorldChecker*>* checkers);
    void start();
    sf::RenderWindow* getWindow();
    ScreenManager(const ScreenManager&) = delete;
    ScreenManager& operator=(const ScreenManager&) = delete;

private:
    ScreenManager();
    ~ScreenManager();
    sf::RenderWindow* window{};
    sf::Font font{};
    void drawBackground() const;
    void drawInfo() const;
};

#endif //SCREENMANAGER_H
