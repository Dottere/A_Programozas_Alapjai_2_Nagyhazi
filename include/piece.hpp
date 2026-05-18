#pragma once

/*
 * Ez a fejlécfájl lesz felelős a bábu ősosztály és az abból származtatott összes többi bábu megvalósításával.
 */

#include "chesstypes.hpp"
#include "constants.hpp"
#include <string>
#include <string_view>
#include <memory>
#include <vector>
#include <array>

namespace PIECE_SYMBOLS
{
        enum class PIECE
    {
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING
    };

    // uppercase for white
    constexpr std::array<std::string_view, 6> ASCII_UPPER = {"P","N","B","R","Q","K"};    
    // lowercase for black
    constexpr std::array<std::string_view, 6> ASCII_LOWER = {"p","n","b","r","q","k"}; 
    
    namespace WHITE
    {
        inline constexpr std::array<std::string_view, 6> UNICODE = {
        "\u2659","\u2658","\u2657","\u2656","\u2655","\u2654"};
    }

    namespace BLACK
    {
        inline constexpr std::array<std::string_view, 6> UNICODE = {
        "\u265F","\u265E","\u265D","\u265C","\u265B","\u265A"
        };
    
    }

    enum class Mode { ASCII, UNICODE };

    inline Mode mode = Mode::UNICODE;

    inline std::string_view getSymbol(PIECE piece, Color color)
    {
        size_t index = static_cast<size_t>(piece);
        if (mode == Mode::ASCII)
        {
            return (color == Color::WHITE) ? ASCII_UPPER[index] : ASCII_LOWER[index];
        }
        else
        {
            return (color == Color::WHITE) ? WHITE::UNICODE[index] : BLACK::UNICODE[index];
        }
    }
}


// forward declaration
class Board;

/**
 * @brief A bábu alaposztály melyet kiterjeszt az összes többi bábu.
 *
 * A bábuk tudják önmagukról, hogy hogyan léphetnek, milyen színűek, léptek-e már. Ezt kezeli ez az osztály.
 */
class Piece
{
    const Color PieceColor;
    bool hasMoved = false;

protected:
    PIECE_SYMBOLS::PIECE pieceEnum;
    char pieceType;
    int value;

    Piece(Color color, PIECE_SYMBOLS::PIECE pEnum, char type, int val) : 
        PieceColor(color), pieceEnum(pEnum), pieceType(type), value(val) {}

    [[nodiscard]] virtual bool checkGeometry(Position<> startPos, Position<> endPos, const Piece *targetPiece) const = 0;

public:
    virtual ~Piece() = default;

    [[nodiscard]] bool isValidMove(Position<> startPos, Position<> endPos, const Piece *TargetPiece) const
    {
        if (startPos.x == endPos.x && startPos.y == endPos.y)
            return false;

        if (TargetPiece != nullptr && TargetPiece->getColor() == this->PieceColor)
            return false;
        return checkGeometry(startPos, endPos, TargetPiece);
    }

    [[nodiscard]] int getValue() const { return value; }
    [[nodiscard]] Color getColor() const { return PieceColor; }
    [[nodiscard]] bool getHasMoved() const { return hasMoved; }
    void setHasMoved(bool val) { hasMoved = val; }
    [[nodiscard]] char getPieceType() const { return pieceType; }
    [[nodiscard]] bool isWhite() const { return PieceColor == Color::WHITE; }
        
    [[nodiscard]] virtual std::vector<Position<>> getPath(Position<> startPos, Position<> endPos) const
    {
        std::vector<Position<>> path;
        int stepX = (endPos.x > startPos.x) ? 1 : ((endPos.x < startPos.x) ? -1 : 0);
        int stepY = (endPos.y > startPos.y) ? 1 : ((endPos.y < startPos.y) ? -1 : 0);

        Position<> current = startPos;
        current.x += stepX;
        current.y += stepY;

        while(current.x != endPos.x || current.y != endPos.y)
        {
            path.push_back(current);
            current.x += stepX;
            current.y += stepY;
        }
        return path;
    }

    [[nodiscard]] std::string_view getSymbol() const { return PIECE_SYMBOLS::getSymbol(pieceEnum, PieceColor); }

    [[nodiscard]] virtual std::unique_ptr<Piece> clone() const = 0;
};

class Rook : public Piece
{
public:
    Rook(Color color) : Piece(color, PIECE_SYMBOLS::PIECE::ROOK, 'R', static_cast<int>(PIECE_VALUES::ROOK)) {}

    [[nodiscard]] std::unique_ptr<Piece> clone() const override
    {
        return std::make_unique<Rook>(*this);
    }

    

protected:
    [[nodiscard]] bool checkGeometry(Position<> startPos, Position<> endPos, const Piece *TargetPiece) const override;
};

class Knight : public Piece
{
public:
    Knight(Color color) : Piece(color, PIECE_SYMBOLS::PIECE::KNIGHT, 'N', static_cast<int>(PIECE_VALUES::KNIGHT)) {}

    [[nodiscard]] std::unique_ptr<Piece> clone() const override
    {
        return std::make_unique<Knight>(*this);
    }

    [[nodiscard]] virtual std::vector<Position<>> getPath(Position<>, Position<>) const override
    {
        return {};
    }

protected:
    [[nodiscard]] bool checkGeometry(Position<> startPos, Position<> endPos, const Piece *TargetPiece) const override;
};

class Bishop : public Piece
{
public:
    Bishop(Color color) : Piece(color, PIECE_SYMBOLS::PIECE::BISHOP, 'B', static_cast<int>(PIECE_VALUES::BISHOP)) {}

    [[nodiscard]] std::unique_ptr<Piece> clone() const override
    {
        return std::make_unique<Bishop>(*this);
    }

protected:
    [[nodiscard]] bool checkGeometry(Position<> startPos, Position<> endPos, const Piece *TargetPiece) const override;
};

class Queen : public Piece
{
public:
    Queen(Color color) : Piece(color, PIECE_SYMBOLS::PIECE::QUEEN, 'Q', static_cast<int>(PIECE_VALUES::QUEEN)) {}

    [[nodiscard]] std::unique_ptr<Piece> clone() const override
    {
        return std::make_unique<Queen>(*this);
    }

protected:
    [[nodiscard]] bool checkGeometry(Position<> startPos, Position<> endPos, const Piece *TargetPiece) const override;
};

class King : public Piece
{
public:
    King(Color color) : Piece(color, PIECE_SYMBOLS::PIECE::KING, 'K', static_cast<int>(PIECE_VALUES::KING)) {}

    [[nodiscard]] std::unique_ptr<Piece> clone() const override
    {
        return std::make_unique<King>(*this);
    }

    [[nodiscard]] virtual std::vector<Position<>> getPath(Position<>, Position<>) const override
    {
        return {};
    }

protected:
    [[nodiscard]] bool checkGeometry(Position<> startPos, Position<> endPos, const Piece *TargetPiece) const override;
};

class Pawn : public Piece
{
public:
    Pawn(Color color) : Piece(color, PIECE_SYMBOLS::PIECE::PAWN, 'P', static_cast<int>(PIECE_VALUES::PAWN)) {}

    [[nodiscard]] std::unique_ptr<Piece> clone() const override
    {
        return std::make_unique<Pawn>(*this);
    }

    [[nodiscard]] virtual std::vector<Position<>> getPath(Position<>, Position<>) const override
    {
        return {};
    }

protected:
    [[nodiscard]] bool checkGeometry(Position<> startPos, Position<> endPos, const Piece *TargetPiece) const override;
};