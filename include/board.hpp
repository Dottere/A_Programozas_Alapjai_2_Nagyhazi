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

        bool isPathClear(int x_start, int y_start, int x_end, int y_end) const;
};

#endif