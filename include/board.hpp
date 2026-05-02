#ifndef BOARD_HPP
#define BOARD_HPP

#include "piece.hpp"

#include <memory>

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
    std::unique_ptr<Piece> board[8][8];
    Color turn = Color::WHITE;

    Position<> findKing(Color c);

    public:
        Board() = default;
        // Speciális másoló konstruktor az std::unique_ptr használata miatt
        Board(const Board& b);
        ~Board() = default;

        /**
         * @brief Beállítja a sakk szabályaiban meghatározott kezdőpozíciót
         * 
         * Használja a loadFromFEN() függvényt, hogy egy előre meghatározott sztringből
         * betöltse az abba foglalt kezdőpozíciót.
         * 
         * @note Ha sikeresen megtörtént a betöltés, akkor std::cout-ra ír visszajelzést, ha nem, akkor std::cerr-re.
         * 
         * @return Igaz/True | Hamis/False
         */
        bool initialSetup();

        /** 
         * @brief Megmondja, hogy a megadott koordináta rajta van-e a táblán.
         * @param x Behatárolja a sort, x koordináta
         * @param y Behatárolja az oszlopot, y koordináta
         * @return True/Igaz ha rajta van, False/Hamis ha nincs.
        */
        inline bool isOnBoard(Position<> pos) const { 
            return (pos.x >= 0 && pos.x <= 7 && pos.y >= 0 && pos.y <= 7); 
        }

        /**
         * @brief Visszaadja az adott koordinátán álló bábut.
         * @param x Behatárolja a sort, x koordináta
         * @param y Behatárolja az oszlopot, y koordináta
         * @return Bábu mutató, vagy nullptr ha nem áll ott bábu
         */
        inline Piece* getPiece(Position<> pos) {
            return board[pos.x][pos.y].get();
        }

        /** 
         * @copydoc getPiece(int, int)  
        */ 
        inline const Piece* getPiece(Position<> pos) const {
            return board[pos.x][pos.y].get();
        }

        /**
         * @brief Megmondja, hogy a lépés a táblán történik-e.
         * @param startPos A kezdő x és y koordináták.
         * @param endPos a végső x és y koordináták.
         * @return True/Igaz, ha mindkettő rajta van, False/Hamis ha nem.
         */
        inline bool isWithinBounds(Position<> startPos, Position<> endPos) const {
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
        bool isPathClear(Position<> startPos, Position<> endPos) const;

        template <int stepX, int stepY>
        bool isPathClear(Position<> startPos, Position<> endPos) const {
            int currX = startPos.x + stepX;
            int currY = startPos.y + stepY;

            while (currX != endPos.x || currY != endPos.y) {
                if (getPiece(Position(currX, currY))) {
                    return false;
                }
                currX += stepX;
                currY += stepY;
            }
            return true;
        }

        /**
         * @brief Megmondja, hogy az adott játékos királya sakkban van-e
         * 
         * Megkeresi a bekért színhez tartozó játékos királyát, majd ha bármelyik ellenkező színű bábú isValidMove() függvénye
         * igazat ad vissza, akkor ez a függvény is igazat ad vissza.
         * 
         * @param c Ahhoz a játékoshoz tartozó szín, aki éppen nincs soron.
         * 
         * @return Igaz/True ha sakkban áll a király, Hamis/False ha nem.
         */
        bool isCheck(Color c);
        /**
         * @brief Megmondja, hogy az adott játékos mattot kapott-e
         * 
         * Kiterjeszti az isCheck() függvényt, és megnézi, hogy meg lehet-e szakítani a sakkot. Ha nem,
         * akkor igazat ad vissza és az ellenkező játékos nyert.
         * @param c Ahhoz a játékoshoz tartozó szín, aki éppen nincs soron.
         * 
         * @return Igaz/True ha nem tud el lépni a király, Hamis/False ha mégis.
         */
        bool isCheckMate(Color c);
        /**
         * @brief Egy függvény ami lehelyez egy bábut egy adott pozícióra
         * 
         * Bekér egy létező bábu-t és egy pozíciót, majd leteszi azt oda. Ha már állt ott bábu, akkor az automatikusan felülíródik
         * és törlődik, ha a bábu már előzőleg is jelen volt a táblán akkor az std::move() és std::unique_ptr kombinációja miatt az
         * el lesz onnan mozdítva és nullptr lesz a helyén.
         * 
         * @param piece A mozdítandó/lehelyezendő bábu
         * @param pos Egy a táblán lévő pozíció
         * 
         * @note Ha a pozíció nem található meg a 8x8-as táblán, akkor automatikusan hamissal tér vissza.
         * 
         * @return Igaz/True ha sikeresen megtörtént, Hamis/False ha nem.
         */
        bool placePiece(std::unique_ptr<Piece> piece, Position<> pos);

        /**
         * @brief Megkeresi egy lépés kezdőpozícióját
         * 
         * Mivel a sakkhoz használt PGN (Portable Game Notation) lusta módon jeleníti meg a lépéseket, ezért nagyon sok
         * kontextusfüggő információt tartalmaz. Emiatt szükséges egy ilyen segédfüggvény amely a bekért paraméterek segítségével
         * meg tudja mondani, hogy mely kezdőpozícióból indult egy lépés. Ha nem a Board osztályon belül lennénk akkor szükséges lenne
         * hozzá egy const Board& referencia is, de mivel lokálisan ez elérhető ezért így nincs szükség
         * azt a paraméterek közt feltüntetni
         * 
         * @param pieceType A bábu típusa (N, B, Q...), szükséges ahhoz, hogy tudjuk mely isValidMove függvényt kell meghívni
         * @param isWhiteToMove Megadja, hogy a világos vagy sötét játékos lép-e. Szükséges, mivel ugyanazon típusú bábuk tartozhatnak
mindkét játékoshoz, de csak a jelenlegi játékos bábui érdekelnek minket.
         * @param endPos A lépéshez tartozó végpozíció, ennek a párját keressük
         * @param fileDisambiguity Ha egyszerre több ugyanazon típusú bábu tud lépni ugyanarra a végpozícióra, akkor a PGN beszúr egy
egyértelműsítő karaktert, amely lehet sor, oszlop, vagy mindkettő, attól függően, hogy hány bábu és milyen felállásban tud odalépni.
         * @param rankDisambiguity Ha egyszerre több ugyanazon típusú bábu tud lépni ugyanarra a végpozícióra, akkor a PGN beszúr egy
egyértelműsítő karaktert, amely lehet sor, oszlop, vagy mindkettő, attól függően, hogy hány bábu és milyen felállásban tud odalépni.
         * @return Visszaadja azt a kezdőpozíciót, amelyről indulhatott az a lépés
         * @note Ha több ilyen pozíció van, vagy egy sincs, akkor ír ki hibát std::cerr-re, és egy a táblán nem létező
-1,-1 pozíciót ad vissza
         */
        Position<> findStartSquare(char pieceType, bool isWhiteToMove, Position<> endPos, char fileDisambiguity, char rankDisambiguity);

        // A belső turn állapotváltozó alapján visszaadja, hogy a fehér játékos van-e soron.
        inline bool isWhiteToMove() const {
            return (turn == Color::WHITE);
        }

        // A belső turn állapotváltozó alapján visszaadja, hogy melyik játékos van most soron.
        inline Color getTurn() const {
            return turn;
        }

        // Teljesen letörli a táblát
        void clearBoard();
        
        /**
         * @brief Betölt egy pozíciót egy FEN-ben kódolt sztringből
         * 
         * A FEN (Forsyth-Edwards Notation) egy olyan jelölési módszer, amely egy fényképet készít a jelenlegi pozícióról. A PGN-nel
         * ellentétben nem kontextusfüggő, bármikor betölthető a tábla teljes állapota belőle.
         * 
         * @param fen A sztring mely tartalmazza a FEN jelölést
         * 
         * @return Igaz/True ha megtörtént a betöltés, Hamis/False ha nem
         */
        bool loadFromFEN(const std::string& fen);

        // Hasonlóképp működik mint a placePiece() függvény, viszont ez nem dolgozik explicit bábumutatóval, hanem csak egy kezdő
        // és egy végpozíciót bekérve teszi meg az áthelyezést.
        inline void movePiece(Position<> startPos, Position<> endPos) {
            board[endPos.x][endPos.y] = std::move(board[startPos.x][startPos.y]);
        }
};

#endif