#include "piece.hpp"
#include <cmath>

// Piece

Color Piece::getColor(void) const {
    return PieceColor;
}

// Rook

bool Rook::isValidMove(int x_start, int y_start, int x_end, int y_end, const Piece* TargetPiece) const {
    if (x_start == x_end && y_start == y_end) return false;

    int dx = std::abs(x_end - x_start);
    int dy = std::abs(y_end - y_start);
    if (!(dx == 0 || dy == 0)) return false;

    if (TargetPiece != nullptr) {
        if (TargetPiece->getColor() == this->getColor()) return false;
    }

    return true;
}

// Knight

bool Knight::isValidMove(int x_start, int y_start, int x_end, int y_end, const Piece* TargetPiece) const {
    if (x_start == x_end && y_start == y_end) return false;

    int dx = std::abs(x_end - x_start);
    int dy = std::abs(y_end - y_start);
    if (
        !( (dx == 2 && dy == 1) || (dx == 1 && dy == 2) )
    ) return false;

    if (TargetPiece != nullptr) {
        if (TargetPiece->getColor() == this->getColor()) return false;
    }

    return true;
}

// Bishop

bool Bishop::isValidMove(int x_start, int y_start, int x_end, int y_end, const Piece* TargetPiece) const {
    if (x_start == x_end && y_start == y_end) return false;

    int dx = std::abs(x_end - x_start);
    int dy = std::abs(y_end - y_start);
    if (dx != dy) return false;

    if (TargetPiece != nullptr) {
        if (TargetPiece->getColor() == this->getColor()) return false;
    }

    return true;

}

// Queen

bool Queen::isValidMove(int x_start, int y_start, int x_end, int y_end, const Piece* TargetPiece) const {
    if (x_start == x_end && y_start == y_end) return false;

    int dx = std::abs(x_end - x_start);
    int dy = std::abs(y_end - y_start);

    if ( !( (dx == 0 || dy == 0) || (dx == dy) ) ) return false;


    if (TargetPiece != nullptr) {
        if (TargetPiece->getColor() == this->getColor()) return false;
    }

    return true;
}

// King

bool King::isValidMove(int x_start, int y_start, int x_end, int y_end, const Piece* TargetPiece) const {
    if (x_start == x_end && y_start == y_end) return false;

    int dx = std::abs(x_end - x_start);
    int dy = std::abs(y_end - y_start);

    if (dx > 1 || dy > 1) return false;

    if (TargetPiece != nullptr) {
        if (TargetPiece->getColor() == this->getColor()) return false;
    }

    return true;
}

// Pawn

bool Pawn::isValidMove(int x_start, int y_start, int x_end, int y_end, const Piece* TargetPiece) const {
    if (x_start == x_end && y_start == y_end) return false;

    int direction = this->getColor() == Color::WHITE ? 1 : -1;
    
    int dx = std::abs(x_end - x_start);
    int dy = y_end - y_start;

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