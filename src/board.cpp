#include "board.hpp"

#include <cmath>

Board::Board() {
    for (int i = 0; i < 8; i++){

        for (int j = 0; j < 8; j++) {
            board[i][j] = nullptr;
        }

    }
}

Board::Board(const Board& b) {
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++) {
            if (b.board[i][j] != nullptr)
                board[i][j] = b.board[i][j]->clone();
            else 
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

bool Board::isPathClear(Position startPos, Position endPos) const {
    int stepX = (endPos.x > startPos.x) ? 1 : ((endPos.x < startPos.x) ? -1 : 0);
    int stepY = (endPos.y > startPos.y) ? 1 : ((endPos.y < startPos.y) ? -1 : 0);

    int currX = startPos.x + stepX;
    int currY = startPos.y + stepY;

    while (currX != endPos.x || currY != endPos.y) {
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