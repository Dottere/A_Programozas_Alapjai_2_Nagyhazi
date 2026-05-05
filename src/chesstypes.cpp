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