#ifndef GAMEMASTER_HPP
#define GAMEMASTER_HPP

#include "board.hpp"
#include "renderer.hpp"
#include "piece.hpp"
#include <vector>
#include <string>

/**
 * @brief Egy lépés adatait tároló struktúra.
 */
struct Move {
    Position<> startPos;
    Position<> endPos;

    bool isCapture;
    bool isCastle;
    bool isEnPassant;
    bool isCheck;

    char movedPiece;
    char promotedTo;

    char capturedPieceType; 
    Color capturedPieceColor;

    // Konstruktor deklaráció

    Move(Position<> start, Position<> end, bool isCapture, 
         bool isCastle, bool isEnPassant, bool isCheck,
         char movedPiece, char promotedTo, Piece* capturedPiece);
};

/**
 * @brief A játék fő vezérléséért felelős osztály.
 */
class GameMaster {
public:
    GameMaster(const Board& board, const Renderer& renderer);

    // --- Fő vezérlő függvények ---
    
    /**
     * @brief A fő játékciklus, ami a bemenetet és a megjelenítést kezeli.
     */
    void gameLoop();

    /**
     * @brief Feldolgoz és érvényesít egy lépést a megadott koordináták között.
     * @return Igaz, ha a lépés szabályos és megtörtént, különben hamis.
     */
    bool processMove(Position<> startPos, Position<> endPos);
};

#endif