#ifndef CHESSTYPES_HPP
#define CHESSTYPES_HPP

#include <iostream>

class Piece;

/**
 * @brief Egy szimpla enum amely nevesíti a bábuk színeit.
 */
enum class Color {
    WHITE,
    BLACK,
    NONE
};
std::ostream& operator<<(std::ostream& os, const Color& c);

/**
 * @brief Az olvashatóság és egyszerűség kedvéért egy az x és y koordinátákat egyesítő struktúra
 */
template <typename T = int>
struct Position{
    T x, y;

    Position() : x(0), y(0) {;}
    Position(T x, T y) : x(x), y(y) {;}

    template <typename U>
    bool operator==(const Position<U>& other) const {
        return x == other.x && y == other.y;
    }

    inline bool isValid() {
        return (x == -1 || y == -1) ? false : true;
    }
};
std::ostream& operator<<(std::ostream& os, const Position<>& p);

struct Move {

    Move(Position<> start, Position<> end, bool isCapture, 
    bool isCastle, bool isEnPassant, bool isCheck,
    char movedPiece, char promotedTo, Piece* capturedPiece);

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

#endif