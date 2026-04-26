#ifndef GAMEMASTER_HPP
#define GAMEMASTER_HPP

#include "board.hpp"
#include "renderer.hpp"
#include "piece.hpp"

#include <vector>
#include <string>
#include <iostream>

struct Move {

    Move(Position<> start, Position<> end, bool isCapture, 
    bool isCastle, bool isEnPassant, bool isCheck,
    char movedPiece, char promotedTo, Piece* capturedPiece) 
    : startPos(start), 
    endPos(end), 
    isCapture(isCapture),
    isCastle(isCastle), 
    isEnPassant(isEnPassant), 
    isCheck(isCheck), 
    movedPiece(movedPiece), 
    promotedTo(promotedTo),
    capturedPieceType('\0'),
    capturedPieceColor(Color::NONE) {

        if (capturedPiece) {
            capturedPieceType = capturedPiece->getPieceType();
            capturedPieceColor = capturedPiece->getColor();
        }

    }
    Position<> startPos;
    Position<> endPos;

    bool isCapture;
    bool isCastle;
    bool isEnPassant;
    bool isCheck;

    char movedPiece;
    char promotedTo;

    char capturedPieceType; // 'n', 'p', 'r'... etc
    Color capturedPieceColor;
};

class GameMaster {
    Board board;
    Renderer renderer;

    std::vector<Move> moveHistory;

public:

    GameMaster(const Board& board, const Renderer& renderer) : board(board), renderer(renderer) {;}

    inline void gameLoop() {

        std::string placeholder;

        while (true) {
            renderer.display();
            // 1. take input in some form, maybe pgn, maybe a more intuitive way for the player (more code more annoyance)
            std::cin >> placeholder;
            // 2. make into coords
            // 3. processMove(startPos, endPos)
            // 4. change turn
        }
    }

    inline bool processMove(Position<> startPos, Position<> endPos) {
        Piece* p = board.getPiece(startPos);
        if (!p || p->getColor() != board.getTurn()) return false;

        Piece* target = board.getPiece(endPos);

        if (p->isValidMove(startPos, endPos, target)) {
            if (p->canJump() || board.isPathClear(startPos, endPos)) {
                // move impl, tbd
                return true;
            }
        }
        return false;
    }
};

#endif