#ifndef BOARD_HPP
#define BOARD_HPP

#include "piece.hpp"
#include <memory>
#include <string>

/**
 * @brief A sakk tábla kezelése és a lépések validálása.
 */
class Board {
public:
    Board();
    Board(const Board& b);
    ~Board();

    // --- Belső segédfüggvények ---

    bool isOnBoard(Position<> pos) const;
    bool isWithinBounds(Position<> startPos, Position<> endPos) const;

    // --- Bábu kezelés ---

    Piece* getPiece(Position<> pos);
    const Piece* getPiece(Position<> pos) const;
    bool placePiece(std::unique_ptr<Piece> piece, Position<> pos);
    void movePiece(Position<> startPos, Position<> endPos);
    void clearBoard();

    // --- Lépés validáció adapter ---

    bool isPathClear(Position<> startPos, Position<> endPos) const;

    // --- Lépés validáció sablonja, amikor ismert a dx, dy. Sokkal gyorsabb így a program. ---

    template <int stepX, int stepY>
    bool isPathClear(Position<> startPos, Position<> endPos) const {
        return true; 
    }

    // --- Játékállapot ellenőrzése ---

    bool isCheck(Color c);
    bool isCheckMate(Color c);

    // --- Lépés feldolgozás-hoz segédfüggvény ---

    Position<> findStartSquare(char pieceType, bool isWhiteToMove,
                               Position<> endPos,
                               char fileDisambiguity,
                               char rankDisambiguity);

    // --- Állapot betöltése ---

    bool loadFromFEN(const std::string& fen);

    // --- Getterek ---

    bool isWhiteToMove() const;
    Color getTurn() const;
};

#endif