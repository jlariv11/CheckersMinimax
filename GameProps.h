//
// Created by Jake Lariviere on 10/20/24.
//

#ifndef GAMEPROPS_H
#define GAMEPROPS_H

#define BOARD_WIDTH 8
#define BOARD_HEIGHT 8
#define BOARD_OFFSET_X 200
#define BOARD_OFFSET_Y 100
#define BOARD_SQUARE_SIZE 50
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define CHECKER_RADIUS 25
enum Player {
    RED=0,
    BLACK=1,
    NONE=-1
};
enum GameState {
    IN_PROGRESS,
    RED_WIN,
    BLACK_WIN,
    DRAW
};

inline Player getOpposite(Player p) {
    switch(p) {
        case RED:
            return BLACK;
        case BLACK:
            return RED;
        default:
            return NONE;
    }
}

#endif //GAMEPROPS_H
