#ifndef CHESSTYPES_HPP
#define CHESSTYPES_HPP

#include <iostream>
#include <string>

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
    bool isCastle, bool isEnPassant, bool isCheck, bool isCheckMate,
    char movedPiece, char promotedTo, Piece* capturedPiece);

    Position<> startPos;
    Position<> endPos;

    bool isCapture;
    bool isCastle;
    bool isEnPassant;
    bool isCheck;
    bool isCheckMate;

    char movedPiece;
    char promotedTo;

    char capturedPieceType; // 'n', 'p', 'r'... etc
    Color capturedPieceColor;
};

struct PGNMetadata {
    std::string event = "?";
    std::string site = "?";
    std::string date = "????.??.??";
    std::string round = "?";
    std::string white = "?";
    std::string black = "?";
    std::string result = "*"; // "*", "1-0", "0-1", or "1/2-1/2"
    std::string setup = "0";
    std::string fen = "";
};

#endif