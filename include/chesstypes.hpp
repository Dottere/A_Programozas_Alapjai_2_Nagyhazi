#ifndef CHESSTYPES_HPP
#define CHESSTYPES_HPP

class Piece;

/**
 * @brief Egy szimpla enum amely nevesíti a bábuk színeit.
 */
enum class Color {
    WHITE,
    BLACK,
    NONE
};

/**
 * @brief Az olvashatóság és egyszerűség kedvéért egy az x és y koordinátákat egyesítő struktúra
 */
template <typename T = int>
struct Position{
    T x, y;

    Position(T x, T y) : x(x), y(y) {;}

    template <typename U>
    bool operator==(const Position<U>& other) const {
        return x == other.x && y == other.y;
    }
};

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