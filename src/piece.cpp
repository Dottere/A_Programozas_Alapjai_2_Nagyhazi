#include "piece.hpp"
#include "board.hpp"
#include <cmath>

// Rook
bool Rook::checkGeometry(Position<> startPos, Position<> endPos, const Piece* TargetPiece) const {
    int dx = std::abs(endPos.x - startPos.x);
    int dy = std::abs(endPos.y - startPos.y);
    
    return (dx == 0 || dy == 0);
}

// Knight
bool Knight::checkGeometry(Position<> startPos, Position<> endPos, const Piece* TargetPiece) const {
    int dx = std::abs(endPos.x - startPos.x);
    int dy = std::abs(endPos.y - startPos.y);
    
    return ((dx == 2 && dy == 1) || (dx == 1 && dy == 2));
}

// Bishop
bool Bishop::checkGeometry(Position<> startPos, Position<> endPos, const Piece* TargetPiece) const {
    int dx = std::abs(endPos.x - startPos.x);
    int dy = std::abs(endPos.y - startPos.y);
    
    return (dx == dy);
}

// Queen
bool Queen::checkGeometry(Position<> startPos, Position<> endPos, const Piece* TargetPiece) const {

    int dx = std::abs(endPos.x - startPos.x);
    int dy = std::abs(endPos.y - startPos.y);

    return ((dx == 0 || dy == 0) || (dx == dy));
}

// King
bool King::checkGeometry(Position<> startPos, Position<> endPos, const Piece* TargetPiece) const {
    int dx = std::abs(endPos.x - startPos.x);
    int dy = std::abs(endPos.y - startPos.y);

    if (dx <= 1 && dy <= 1) return true;

    if (dy == 0 && dx == 2 && !getHasMoved()) return true;

    return false;
}

// Pawn
bool Pawn::checkGeometry(Position<> startPos, Position<> endPos, const Piece* TargetPiece) const {

    int direction = isWhite() ? 1 : -1;
    
    int dx = std::abs(endPos.x - startPos.x);
    int dy = endPos.y - startPos.y;

    if (dx == 0) {

        if (TargetPiece) return false;
        
        if (dy == direction) return true;

        if (dy == 2 * direction && !getHasMoved()) return true;

        return false;

    } else if (dx == 1) {
        if (dy == direction && TargetPiece) return true;

        return false;
    }

    return false;
}