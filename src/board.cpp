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

Position Board::findKing(Color c) {
    Position kingPos = Position(0,0);

    for (int y = 7; y >= 0; y--) {
        for (int x = 0; x < 8; x++) {
            if (board[x][y]->getColor() == c)
                if (board[x][y]->isKing())
                    kingPos = Position(x,y);
        }
    }

    return kingPos;
}

bool Board::isCheck(Color c) {
    Position kingPos = findKing(c);
    Piece* king = board[kingPos.x][kingPos.y];

    for (int y = 7; y >= 0; y--) {
        for (int x = 0; x < 8; x++) {
            if (board[x][y]->isValidMove(Position(x, y), kingPos, king)) return true;
        }
    }
    return false;
}

bool Board::isCheckMate(Color c) {
    Position kingPos = findKing(c);
    Piece* king = board[kingPos.x][kingPos.y];

    bool isCheckMate = true;

    if (!isCheck(c)) return false;

    for (int y = kingPos.y+1; y >= kingPos.y-1; y--) {
        for (int x = kingPos.x-1; x <= kingPos.x+1; x++) {
            if (!isOnBoard(Position(x,y))) continue;

            if (king->isValidMove(kingPos, Position(x,y), board[x][y])) isCheckMate = false; 
        }
    }
    return isCheckMate;
}
