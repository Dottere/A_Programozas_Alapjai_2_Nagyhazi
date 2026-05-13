#include "chesstypes.hpp"

#include "piece.hpp"

Move::Move(Position<> start, Position<> end, Flags f,
    char movedPiece, char promotedTo, Piece* capturedPiece) : 
    startPos(start), 
    endPos(end), 
    capturedPieceType('\0'),
    capturedPieceColor(Color::NONE) {
        flags = {0};

        if (capturedPiece) {
            capturedPieceType = capturedPiece->getPieceType();
            capturedPieceColor = capturedPiece->getColor();
        }
}

std::ostream& operator<<(std::ostream& os, const Move& move) {
    os << "Start: " << move.startPos << " End: " << move.endPos << " isCapture: " << move.flags.isCapture 
    << " isCastle: " << move.flags.isCastle << " isEnPassant: " << move.flags.isEnPassant << " IsCheck: "<< move.flags.isCheck
    << " isCheckMate: " << move.flags.isCheckMate << " movedPiece: " << move.movedPiece 
    << " promotedTo: " << move.promotedTo << " CapturedPieceType: "
    << move.capturedPieceType << " capturedPieceColor: " << move.capturedPieceColor;

    return os;
}

std::ostream& operator<<(std::ostream& os, const Color& c) {
    switch (c) {
        case Color::WHITE: os << "Fehér"; break;
        case Color::BLACK: os << "Sötét"; break;
        case Color::NONE:  os << "Üres"; break;
    }
    return os;
}