#ifndef BOARD_HPP
#define BOARD_HPP

#include "piece.hpp"

/*
* Ez a fejlécfájl lesz felelős a tábla kezeléséért, a bábuk reprezentációjáért. A tábla önmagában egy 8x8-as tömb,
* amely Bábu mutatókat tartalmaz. Ezen felül felelős lesz a PGN értelmezéséért.
*/

/**
* @brief Ez az osztály felelős a tábla állapotáért és a lépések validálásáért.
*
* A tábla osztály tartalmazza azt a 8x8 cellát amelyre a bábuk léphetnek, minden lépéskor szimulálja azt, majd ha az valid --
* tehát nem ugrunk át csúszó bábukkal más bábukat, vagy nem ugrunk le a tábláról -- akkor megteszi azt.
*/
class Board {
    // A tábla 8x8-as tömbje, mely Bábu osztály mutatókat tartalmaz.
    Piece* board[8][8];

    public:
        Board();
        Board(const Board& b);
        ~Board();

        /** 
         * @brief Megmondja, hogy a megadott koordináta rajta van-e a táblán.
         * @param x Behatárolja a sort, x koordináta
         * @param y Behatárolja az oszlopot, y koordináta
         * @return True/Igaz ha rajta van, False/Hamis ha nincs.
        */
        inline bool isOnBoard(Position pos) const { 
            return (pos.x >= 0 && pos.x <= 7 && pos.y >= 0 && pos.y <= 7); 
        }

        /**
         * @brief Visszaadja az adott koordinátán álló bábut.
         * @param x Behatárolja a sort, x koordináta
         * @param y Behatárolja az oszlopot, y koordináta
         * @return Bábu mutató, vagy nullptr ha nem áll ott bábu
         */
        inline Piece* getPiece(Position pos) {
            return board[pos.x][pos.y];
        }

        /** 
         * @copydoc getPiece(int, int)  
        */ 
        inline const Piece* getPiece(Position pos) const {
            return board[pos.x][pos.y];
        }

        /**
         * @brief Megmondja, hogy a lépés a táblán történik-e.
         * @param startPos A kezdő x és y koordináták.
         * @param endPos a végső x és y koordináták.
         * @return True/Igaz, ha mindkettő rajta van, False/Hamis ha nem.
         */
        inline bool isWithinBounds(Position startPos, Position endPos) const {
            return (isOnBoard(startPos) && isOnBoard(endPos));
        }
        
        /**
         * @brief Megmondja, hogy a bábu útjában áll-e más bábu.
         * @param x_start A kezdő x koordináta
         * @param y_start A kezdő y koordináta
         * @param x_end A végső x koordináta
         * @param y_end A végső y koordináta
         * @return True/Igaz, ha mindkettő rajta van, False/Hamis ha nem.
         */
        bool isPathClear(int x_start, int y_start, int x_end, int y_end) const;

        bool isCheck(Color c);
        bool isCheckMate(Color c);

        bool placePiece(Piece& piece, Position pos);
};

#endif