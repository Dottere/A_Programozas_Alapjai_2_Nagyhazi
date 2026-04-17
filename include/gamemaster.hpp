#ifndef GAMEMASTER_HPP
#define GAMEMASTER_HPP

#include "board.hpp"
#include "renderer.hpp"
#include "piece.hpp"
#include <vector>

/*
NOT YET FINISHED
*/

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
            // 1. Bemenet bekérése (pl. "e2 e4")
            // 2. Koordinátákra bontás
            // 3. processMove(x1, y1, x2, y2)
            // 4. Játékos váltása, ha sikeres volt a lépés
        }
    }

    bool processMove(int x1, int y1, int x2, int y2) {
        Piece* p = board.getPiece(x1, y1);
        if (!p || p->getColor() != turn) return false;

        Piece* target = board.getPiece(x2, y2);

        if (p->isValidMove(x1, y1, x2, y2, target)) {
            // A huszárnak nem kell isPathClear, mindenki másnak igen
            // (Ezért is jó az isPathClear-ben a currX != x_end feltétel!)
            if (board.isPathClear(x1, y1, x2, y2)) {
                // Itt történne meg a tényleges mozgatás a board tömbben
                return true;
            }
        }
        return false;
    }
};

#endif