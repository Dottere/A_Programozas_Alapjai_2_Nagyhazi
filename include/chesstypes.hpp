#pragma once

#include <iostream>
#include <cstdint>
#include <string>

class Piece;

/**
 * @brief Egy szimpla enum amely nevesíti a bábuk színeit.
 */
enum class Color
{
    WHITE,
    BLACK,
    NONE
};
std::ostream &operator<<(std::ostream &os, const Color &c);

/**
 * @brief Az olvashatóság és egyszerűség kedvéért egy az x és y koordinátákat egyesítő struktúra
 */
template <typename T = int>
struct Position
{
    T x, y;

    Position() : x(0), y(0) { ; }
    Position(T x, T y) : x(x), y(y) { ; }

    template <typename U>
    bool operator==(const Position<U> &other) const
    {
        return x == other.x && y == other.y;
    }

    inline bool isValid()
    {
        return (x == -1 || y == -1) ? false : true;
    }
};

template <typename T = int>
std::ostream &operator<<(std::ostream &os, const Position<T> &p)
{
    os << "{" << p.x << ", " << p.y << "}";
    return os;
}

struct Move
{

    Position<> startPos;
    Position<> endPos;

    struct Flags
    {
        uint8_t isCapture : 1;
        uint8_t isCastle : 1;
        uint8_t isEnPassant : 1;
        uint8_t isCheck : 1;
        uint8_t isCheckMate : 1;
        uint8_t reserved : 2;
    } flags;

    char movedPiece;
    char promotedTo;

    char capturedPieceType; // 'n', 'p', 'r'... etc
    Color capturedPieceColor;

    Move(Position<> start = {-1, -1}, Position<> end = {-1, -1}, Flags f = {0},
         char movedPiece = '\0', char promotedTo = '\0', Piece *capturedPiece = nullptr);
};
std::ostream& operator<<(std::ostream &os, const Move &move);

struct PGNMetadata
{
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