#ifndef GAMEMASTER_HPP
#define GAMEMASTER_HPP

#include "board.hpp"
#include "renderer.hpp"
#include "piece.hpp"
#include <vector>

struct Move {
    int start_x, start_y;
    int end_x, end_y;

    bool isCapture;
    bool isCastle;
    bool isEnPassant;
    bool isCheck;

    char movedPiece;
    char promotedTo;

    Piece* capturedPiece;
};

class GameMaster {
    Board board;
    Renderer renderer;
    Color turn = Color::WHITE;

    std::vector<Move> moveHistory;

public:
    void gameLoop() {
        while (true) {
            renderer.display();
            // 1. take input in some form, maybe pgn, maybe a more intuitive way for the player (more code more annoyance)
            // 2. make into coords
            // 3. processMove(x1, y1, x2, y2)
            // 4. change turn
        }
    }

    bool processMove(Position startPos, Position endPos) {
        Piece* p = board.getPiece(startPos);
        if (!p || p->getColor() != turn) return false;

        Piece* target = board.getPiece(endPos);

        if (p->isValidMove(startPos.x, startPos.y, endPos.x, endPos.y, target)) {
            // horse doesn't need Board::isPathClear !!
            if (board.isPathClear(startPos.x, startPos.y, endPos.x, endPos.y)) {
                // physically move
                return true;
            }
        }
        return false;
    }
};

#endif