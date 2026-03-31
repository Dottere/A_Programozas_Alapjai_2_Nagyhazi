#ifndef BOARD_HPP
#define BOARD_HPP

#include "piece.hpp"

/*
* Ez a fejlécfájl lesz felelős a tábla kezeléséért, a bábuk reprezentációjáért. A tábla önmagában egy 8x8-as tömb,
* amely Bábu mutatókat tartalmaz. Ezen felül felelős lesz a PGN értelmezéséért.
*/

class Board {
    Piece* board[8][8];

    public:
        Board();
        ~Board();

        inline bool isOnBoard(int x, int y) const { 
            return (x >= 0 && x <= 7 && y >= 0 && y <= 7); 
        }

        inline Piece* getPiece(int x, int y) {
            return board[x][y];
        }

        inline bool isWithinBounds(int x_start, int y_start, int x_end, int y_end) const {
            return (isOnBoard(x_start, y_start) && isOnBoard(x_end, y_end));
        }
        
        bool isPathClear(int x_start, int y_start, int x_end, int y_end) const;

};

#endif