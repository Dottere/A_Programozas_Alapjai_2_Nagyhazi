#include "piece.hpp"

#include <cmath>

// Rook

bool Rook::isValidMove(Position startPos, Position endPos, const Piece* TargetPiece) const {
    if (startPos.x == endPos.x && startPos.x == endPos.y) return false;

    int dx = std::abs(endPos.x - startPos.x);
    int dy = std::abs(endPos.y - startPos.x);
    if (!(dx == 0 || dy == 0)) return false;

    if (TargetPiece != nullptr) {
        if (TargetPiece->getColor() == this->getColor()) return false;
    }

    return true;
}

// Knight

bool Knight::isValidMove(Position startPos, Position endPos, const Piece* TargetPiece) const {
    if (startPos.x == endPos.x && startPos.x == endPos.y) return false;

    int dx = std::abs(endPos.x - startPos.x);
    int dy = std::abs(endPos.y - startPos.x);
    if (
        !( (dx == 2 && dy == 1) || (dx == 1 && dy == 2) )
    ) return false;

    if (TargetPiece != nullptr) {
        if (TargetPiece->getColor() == this->getColor()) return false;
    }

    return true;
}

// Bishop

bool Bishop::isValidMove(Position startPos, Position endPos, const Piece* TargetPiece) const {
    if (startPos.x == endPos.x && startPos.x == endPos.y) return false;

    int dx = std::abs(endPos.x - startPos.x);
    int dy = std::abs(endPos.y - startPos.x);
    if (dx != dy) return false;

    if (TargetPiece != nullptr) {
        if (TargetPiece->getColor() == this->getColor()) return false;
    }

    return true;

}

// Queen

bool Queen::isValidMove(Position startPos, Position endPos, const Piece* TargetPiece) const {
    if (startPos.x == endPos.x && startPos.x == endPos.y) return false;

    int dx = std::abs(endPos.x - startPos.x);
    int dy = std::abs(endPos.y - startPos.x);

    if ( !( (dx == 0 || dy == 0) || (dx == dy) ) ) return false;


    if (TargetPiece != nullptr) {
        if (TargetPiece->getColor() == this->getColor()) return false;
    }

    return true;
}

// King

bool King::isValidMove(Position startPos, Position endPos, const Piece* TargetPiece) const {
    if (startPos.x == endPos.x && startPos.x == endPos.y) return false;

    int dx = std::abs(endPos.x - startPos.x);
    int dy = std::abs(endPos.y - startPos.x);

    if (dx > 1 || dy > 1) return false;

    if (TargetPiece != nullptr) {
        if (TargetPiece->getColor() == this->getColor()) return false;
    }

    return true;
}

// Pawn

bool Pawn::isValidMove(Position startPos, Position endPos, const Piece* TargetPiece) const {
    if (startPos.x == endPos.x && startPos.x == endPos.y) return false;

    int direction = this->getColor() == Color::WHITE ? 1 : -1;
    
    int dx = std::abs(endPos.x - startPos.x);
    int dy = endPos.y - startPos.x;

    if (dx == 0) {
        if (TargetPiece != nullptr) return false;
        if (!(dy == direction || (dy == 2 * direction && !this->getHasMoved()))) return false;
    }

    if (dx == 1) {
        if (dy != direction) return false;
        if (TargetPiece == nullptr) return false;
        if (TargetPiece->getColor() == this->getColor()) return false;
    }

    if (dx > 1) return false;

    return true;
}