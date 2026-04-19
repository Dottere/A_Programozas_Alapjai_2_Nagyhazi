#include "board.hpp"

#include <cmath>

Board::Board() {
    for (int i = 0; i < 8; i++){

        for (int j = 0; j < 8; j++) {
            board[i][j] = nullptr;
        }

    }
}

Board::~Board() {
        for (int i = 0; i < 8; i++){

        for (int j = 0; j < 8; j++) {
            delete board[i][j];
        }

    }
}

bool Board::isPathClear(int x_start, int y_start, int x_end, int y_end) const {
    int stepX = (x_end > x_start) ? 1 : ((x_end < x_start) ? -1 : 0);
    int stepY = (y_end > y_start) ? 1 : ((y_end < y_start) ? -1 : 0);

    int currX = x_start + stepX;
    int currY = y_start + stepY;

    while (currX != x_end || currY != y_end) {
        if (board[currX][currY] != nullptr) {
            return false;
        }
        currX += stepX;
        currY += stepY;
    }

    return true;
}

bool Board::placePiece(Piece& piece, Position pos) {
    if (!isOnBoard(pos)) return false;

    board[pos.x][pos.y] = &piece;
    return true;
}