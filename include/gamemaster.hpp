#ifndef GAMEMASTER_HPP
#define GAMEMASTER_HPP

#include "board.hpp"
#include "renderer.hpp"
#include "piece.hpp"
#include <vector>

struct Move {

    Move(Position start, Position end, bool isCapture, bool isCastle, bool isEnPassant, bool isCheck,
    char movedPiece, char promotedTo, Piece* capturedPiece) : startPos(start), endPos(end), isCapture(isCapture),
    isCastle(isCastle), isEnPassant(isEnPassant), isCheck(isCheck), movedPiece(movedPiece), promotedTo(promotedTo),
    capturedPiece(capturedPiece) {;}

    Position startPos;
    Position endPos;

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
            // 3. processMove(startPos, endPos)
            // 4. change turn
        }
    }

    bool processMove(Position startPos, Position endPos) {
        Piece* p = board.getPiece(startPos);
        if (!p || p->getColor() != turn) return false;

        Piece* target = board.getPiece(endPos);

        if (p->isValidMove(startPos, endPos, target)) {
            // horse doesn't need Board::isPathClear !!
            if (board.isPathClear(startPos, endPos)) {
                // physically move
                return true;
            }
        }
        return false;
    }
};

#endif