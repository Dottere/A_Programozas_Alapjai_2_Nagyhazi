#include "chesstypes.hpp"

#include "piece.hpp"

Move::Move(Position<> start, Position<> end, bool isCapture, 
    bool isCastle, bool isEnPassant, bool isCheck,
    char movedPiece, char promotedTo, Piece* capturedPiece) : 
    startPos(start), 
    endPos(end), 
    isCapture(isCapture),
    isCastle(isCastle), 
    isEnPassant(isEnPassant), 
    isCheck(isCheck), 
    movedPiece(movedPiece), 
    promotedTo(promotedTo),
    capturedPieceType('\0'),
    capturedPieceColor(Color::NONE) {

        if (capturedPiece) {
            capturedPieceType = capturedPiece->getPieceType();
            capturedPieceColor = capturedPiece->getColor();
        }
}

std::ostream& operator<<(std::ostream& os, const Color& c) {
    std::string color = c == Color::WHITE ? "White" : "Black";
    os << color;
    return os;
}

std::ostream& operator<<(std::ostream& os, const Position<>& p) {
    os << "x: " << p.x << " y: " << p.y;
    return os;
}