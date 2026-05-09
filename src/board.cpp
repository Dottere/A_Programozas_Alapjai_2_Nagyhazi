#include "board.hpp"

#include "piece.hpp"

#include <cmath>
#include <vector>
#include <iostream> 
#include <sstream>
#include <cctype>

Board::Board(const Board& b) {
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++) {
            if (b.board[i][j]) {
                this->board[i][j] = std::move(b.board[i][j]->clone());
            } else {
                this->board[i][j] = nullptr;
            }
        }
    }

    this->turn = b.turn;

    this->whiteCaptured.clear();
    this->blackCaptured.clear();

    this->canWhiteCastleKingside = b.canWhiteCastleKingside;
    this->canWhiteCastleQueenside = b.canWhiteCastleQueenside;
    this->canBlackCastleKingside = b.canBlackCastleKingside;
    this->canBlackCastleQueenside = b.canBlackCastleQueenside;

    this->enPassantTarget = b.enPassantTarget;

    this->halfMoveClock = b.halfMoveClock;
    this->fullMoveNumber = b.fullMoveNumber;
}

bool Board::initialSetup(std::string FENString) {
            if (loadFromFEN(FENString)) {
                std::cout << "Kezdőállapot sikeresen betöltve!\n\n";
                return true;
            } else {
                std::cerr << "Hiba a FEN betöltése során!\n";
                return false;
            }
}

bool Board::isPathClear(Position<> startPos, Position<> endPos) const {
    int stepX = (endPos.x > startPos.x) ? 1 : ((endPos.x < startPos.x) ? -1 : 0);
    int stepY = (endPos.y > startPos.y) ? 1 : ((endPos.y < startPos.y) ? -1 : 0);

    if (stepX == 1) {
        if (stepY == 0)  return isPathClear<1, 0>(startPos, endPos);  // Right
        if (stepY == 1)  return isPathClear<1, 1>(startPos, endPos);  // Up-Right
        if (stepY == -1) return isPathClear<1, -1>(startPos, endPos); // Down-Right
    } else if (stepX == -1) {
        if (stepY == 0)  return isPathClear<-1, 0>(startPos, endPos); // Left
        if (stepY == 1)  return isPathClear<-1, 1>(startPos, endPos); // Up-Left
        if (stepY == -1) return isPathClear<-1, -1>(startPos, endPos);// Down-Left
    } else if (stepX == 0) {
        if (stepY == 1)  return isPathClear<0, 1>(startPos, endPos);  // Up
        if (stepY == -1) return isPathClear<0, -1>(startPos, endPos); // Down
    }

    return true; 
}



bool Board::placePiece(std::unique_ptr<Piece> piece, Position<> pos) {
    if (!isOnBoard(pos)) return false;

    // A tömb a bal felső sarokban kezdi az indexelést, míg a sakk szabályai a bal alsó sarokban
    auto arrayY = 7 - pos.y;

    board[pos.x][arrayY] = std::move(piece);
    return true;
}

Position<> Board::findKing(Color c) {
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            Position<> pos(x, y);
            Piece* p = getPiece(pos);
            
            if (p != nullptr && p->getColor() == c && p->isKing()) {
                return pos;
            }
        }
    }

    return Position<>(-1, -1);
}

bool Board::isCheck(Color c) {
    Position<> kingPos = findKing(c);
    if (!kingPos.isValid()) return false;

    Piece* kingPiece = getPiece(kingPos);

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            Position<> attackerPos(x, y);
            Piece* attacker = getPiece(attackerPos);

            if (attacker && attacker->getColor() != c) {
                if (attacker->isValidMove(attackerPos, kingPos, kingPiece)) {
                    if (attacker->canJump() || isPathClear(attackerPos, kingPos)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool Board::hasLegalMoves(Color c) {
    for (int startY = 7; startY >= 0; startY--) {
        for (int startX = 0; startX < 8; startX++) {
            Position<> startPos(startX, startY);
            Piece* p = getPiece(startPos);

            if (p == nullptr || p->getColor() != c) continue;

            for (int endY = 7; endY >= 0; endY--) {
                for (int endX = 0; endX < 8; endX++) {
                    Position<> endPos(endX, endY);
                    Piece* target = getPiece(endPos);

                    if (target && target->getColor() == c) continue;

                    if (!p->isValidMove(startPos, endPos, target)) continue;
                    if (!p->canJump() && !isPathClear(startPos, endPos)) continue;

                    Board clonedBoard(*this);
                    clonedBoard.movePiece(startPos, endPos);

                    if (!clonedBoard.isCheck(c)) {
                        return true;
                    }
                }
            }
        }
    }
    return false; 
}

bool Board::isCheckMate(Color c) {
    return isCheck(c) && !hasLegalMoves(c);
}

bool Board::isStaleMate(Color c) {
    return !isCheck(c) && !hasLegalMoves(c);
}

Position<> Board::findStartSquare(char pieceType, bool isWhiteToMove, Position<> endPos, char fileDisambiguation, char rankDisambiguation) {
    std::vector<Position<>> candidates;

    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            Position<> currentPos(x, y);
            Piece* p = getPiece(currentPos);

            if (p && p->getPieceType() == pieceType && p->isWhite() == isWhiteToMove) {

                if (fileDisambiguation != '\0' && (x + 'a') != fileDisambiguation) continue;
                if (rankDisambiguation != '\0' && (y + '1') != rankDisambiguation) continue;

                if (p->isValidMove(currentPos, endPos, getPiece(endPos))) {
                    if (pieceType == 'N' || isPathClear(currentPos, endPos)) {
                        candidates.push_back(currentPos);
                    }
                }
            }
        }   
    }

    if (candidates.size() == 1) {
        return candidates[0];
    } else if (candidates.size() > 1) {
        std::cerr << "Hiba: Nem lehet eldönteni a PGN alapján, hogy melyik a helyes lépés! Lehet, hogy hibás?" << std::endl;
        return Position<>{-1, -1};
    } else {
        std::cerr << "Hiba: Nem található olyan bábu, amellyel megtehető ez a lépés!" << std::endl;
        return Position<>{-1, -1};
    }

}

void Board::clearBoard() {
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            board[x][y].reset();
        }
    }

    turn = Color::WHITE;
    canWhiteCastleKingside = false;
    canWhiteCastleQueenside = false;
    canBlackCastleKingside = false;
    canBlackCastleQueenside = false;
    enPassantTarget = Position<>{-1, -1};
    halfMoveClock = 0;
    fullMoveNumber = 1;
}

bool Board::loadFromFEN(const std::string& fen) {
    clearBoard();
    std::stringstream ss(fen);

    std::string placement, activeColor, castling, enPassant;
    int halfMoveClock = 0, fullMoveNumber = 1;

    // Mivel a FEN-ben szóközökkel vannak elválasztva a releváns blokkok ezért a stringstream használata megkönnyíti a feldolgozást
    ss >> placement >> activeColor >> castling >> enPassant >> halfMoveClock >> fullMoveNumber;

    int x = 0;
    int y = 7;

    // Bábuk elhelyezése
    for (char c : placement) {
        if (c == '/') {
            y--;
            x = 0;
        } else if (std::isdigit(c)) {
            x += (c - '0');
        } else {
            Color color = std::isupper(c) ? Color::WHITE : Color::BLACK;
            char type = std::tolower(c);
            Piece* p = nullptr;
            
            switch (type) {
                case 'r' : p = new Rook(color); break;
                case 'n' : p = new Knight(color); break;
                case 'b' : p = new Bishop(color); break;
                case 'p' : p = new Pawn(color); break;
                case 'q' : p = new Queen(color); break;
                case 'k' : p = new King(color); break;
            }

            if (p != nullptr) {
                placePiece(std::unique_ptr<Piece>(p), Position<>(x, y));
                x++;
            }
        }

    }
    
    // Soron következő játékos beállítása
    if (!activeColor.empty()) {
        turn = (activeColor[0] == 'w') ? Color::WHITE : Color::BLACK;
    }
    
    // Sáncolási jogok
    if (castling != "-") {
        for (char c : castling) {
            switch (c) { 
                case 'K': canWhiteCastleKingside = true; break;
                case 'Q': canWhiteCastleQueenside = true; break;
                case 'k': canBlackCastleKingside = true; break;
                case 'q': canBlackCastleQueenside = true; break;
            }
        }
    }

    // En Passant
    if (enPassant != "-" && enPassant.length() == 2) {
        int epX = enPassant[0] - 'a';
        int epY = enPassant[1] - '1';
        enPassantTarget = Position<>{epX, epY};
    }

    return true;
}

