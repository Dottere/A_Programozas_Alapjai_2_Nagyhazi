#include "chesstypes.hpp"

#include "piece.hpp"

Move::Move(Position<> start, Position<> end, bool isCapture, 
    bool isCastle, bool isEnPassant, bool isCheck, bool isCheckMate,
    char movedPiece, char promotedTo, Piece* capturedPiece) : 
    startPos(start), 
    endPos(end), 
    isCapture(isCapture),
    isCastle(isCastle), 
    isEnPassant(isEnPassant), 
    isCheck(isCheck), 
    isCheckMate(isCheckMate),
    movedPiece(movedPiece), 
    promotedTo(promotedTo),
    capturedPieceType('\0'),
    capturedPieceColor(Color::NONE) {

        if (capturedPiece) {
            capturedPieceType = capturedPiece->getPieceType();
            capturedPieceColor = capturedPiece->getColor();
        }
}

std::ostream& operator<<(std::ostream& os, const Move& move) {
    os << "Start: " << move.startPos << " End: " << move.endPos << " isCapture: " << move.isCapture 
    << " isCastle: " << move.isCastle << " isEnPassant: " << move.isEnPassant << " IsCheck: "<< move.isCheck
    << " isCheckMate: " << move.isCheckMate << " movedPiece: " << move.movedPiece 
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