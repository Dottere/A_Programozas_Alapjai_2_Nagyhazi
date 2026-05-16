#pragma once

/*
 * Ez a fejlécfájl lesz felelős a bábu ősosztály és az abból származtatott összes többi bábu megvalósításával.
 */

#include "chesstypes.hpp"
#include "constants.hpp"
#include <string>
#include <string_view>
#include <memory>

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
    std::string_view symbol;
    char pieceType;
    int value;

    Piece(Color color, std::string_view symbol, char type, int val) : PieceColor(color), symbol(std::move(symbol)), pieceType(type), value(val) {}

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

    [[nodiscard]] bool isKing() const { return pieceType == 'K'; }
    [[nodiscard]] int getValue() const { return value; }
    [[nodiscard]] Color getColor() const { return PieceColor; }
    [[nodiscard]] bool getHasMoved() const { return hasMoved; }
    void setHasMoved(bool val) { hasMoved = val; }
    [[nodiscard]] char getPieceType() const { return pieceType; }
    [[nodiscard]] bool isWhite() const { return PieceColor == Color::WHITE; }
    [[nodiscard]] virtual bool canJump() const { return false; }

    [[nodiscard]] std::string_view getSymbol() const { return symbol; }

    [[nodiscard]] virtual std::unique_ptr<Piece> clone() const = 0;
};

class Rook : public Piece
{
public:
    Rook(Color color) : Piece(color, (color == Color::WHITE) ? "\u2656" : "\u265C", 'R', static_cast<int>(PIECE_VALUES::ROOK)) {}

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
    Knight(Color color) : Piece(color, (color == Color::WHITE) ? "\u2658" : "\u265E", 'N', static_cast<int>(PIECE_VALUES::KNIGHT)) {}

    [[nodiscard]] std::unique_ptr<Piece> clone() const override
    {
        return std::make_unique<Knight>(*this);
    }

    bool canJump() const override
    {
        return true;
    }

protected:
    [[nodiscard]] bool checkGeometry(Position<> startPos, Position<> endPos, const Piece *TargetPiece) const override;
};

class Bishop : public Piece
{
public:
    Bishop(Color color) : Piece(color, (color == Color::WHITE) ? "\u2657" : "\u265D", 'B', static_cast<int>(PIECE_VALUES::BISHOP)) {}

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
    Queen(Color color) : Piece(color, (color == Color::WHITE) ? "\u2655" : "\u265B", 'Q', static_cast<int>(PIECE_VALUES::QUEEN)) {}

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
    King(Color color) : Piece(color, (color == Color::WHITE) ? "\u2654" : "\u265A", 'K', static_cast<int>(PIECE_VALUES::KING)) {}

    [[nodiscard]] std::unique_ptr<Piece> clone() const override
    {
        return std::make_unique<King>(*this);
    }

protected:
    [[nodiscard]] bool checkGeometry(Position<> startPos, Position<> endPos, const Piece *TargetPiece) const override;
};

class Pawn : public Piece
{
public:
    Pawn(Color color) : Piece(color, (color == Color::WHITE) ? "\u2659" : "\u265F", 'P', static_cast<int>(PIECE_VALUES::PAWN)) {}

    [[nodiscard]] std::unique_ptr<Piece> clone() const override
    {
        return std::make_unique<Pawn>(*this);
    }

protected:
    [[nodiscard]] bool checkGeometry(Position<> startPos, Position<> endPos, const Piece *TargetPiece) const override;
};