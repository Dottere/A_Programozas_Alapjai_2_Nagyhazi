#include "board.hpp"

#include <cmath>
#include <vector>
#include <iostream> 

Board::Board() {
    for (int i = 0; i < 8; i++){

        for (int j = 0; j < 8; j++) {
            board[i][j] = nullptr;
        }

    }
}

Board::Board(const Board& b) {
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++) {
            if (b.board[i][j] != nullptr)
                board[i][j] = b.board[i][j]->clone();
            else 
                board[i][j] = nullptr;
        }
    }
}

Board::~Board() {
        for (int i = 0; i < 8; i++){

        for (int j = 0; j < 8; j++) {
            delete board[i][j];
        }

    }
}

bool Board::isPathClear(Position startPos, Position endPos) const {
    int stepX = (endPos.x > startPos.x) ? 1 : ((endPos.x < startPos.x) ? -1 : 0);
    int stepY = (endPos.y > startPos.y) ? 1 : ((endPos.y < startPos.y) ? -1 : 0);

    int currX = startPos.x + stepX;
    int currY = startPos.y + stepY;

    while (currX != endPos.x || currY != endPos.y) {
        if (board[currX][currY] != nullptr) {
            return false;
        }
        currX += stepX;
        currY += stepY;
    }
    return true;
}

bool Board::placePiece(Piece* piece, Position pos) {
    if (!isOnBoard(pos)) return false;

    board[pos.x][pos.y] = piece;
    return true;
}

Position Board::findKing(Color c) {
    for (int y = 7; y >= 0; y--) {
        for (int x = 0; x < 8; x++) {
            if (board[x][y] != nullptr) {
                if (board[x][y]->getColor() == c && board[x][y]->isKing()) {
                    return Position(x,y);
                }
            }
        }
    }

    return Position(-1, -1);
}

bool Board::isCheck(Color c) {
    Position kingPos = findKing(c);
    if (kingPos.x == -1) return false;

    for (int y = 7; y >= 0; y--) {
        for (int x = 0; x < 8; x++) {

            Piece* attacker = board[x][y];

            if (attacker != nullptr && attacker->getColor() != c) {
                if (attacker->getPieceType() == 'N' || isPathClear(Position(x,y), kingPos)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Board::isCheckMate(Color c) {
    if (!isCheck(c)) return false;

    for (int startY = 7; startY >= 0; startY--) {
        for (int startX = 0; startX < 8; startX++) {
            Position startPos(startX, startY);
            Piece* p = getPiece(startPos);

            if (p == nullptr || p->getColor() != c) continue;

            for (int endY = 7; endY >= 0; endY--) {
                for (int endX = 0; endX < 8; endX++) {
                    Position endPos(endX, endY);
                    Piece* target = getPiece(endPos);

                    if (!p->isValidMove(startPos, endPos, target)) continue;

                    if (!p->canJump() && !isPathClear(startPos, endPos)) continue;

                    Board clonedBoard(*this);

                    Piece* clonedPiece = clonedBoard.getPiece(startPos);
                    Piece* targetToCapture = clonedBoard.getPiece(endPos);

                    clonedBoard.board[endPos.x][endPos.y] = clonedPiece;
                    clonedBoard.board[startPos.x][startPos.y] = nullptr;

                    if (targetToCapture != nullptr) {
                        delete targetToCapture;
                    }

                    if (!clonedBoard.isCheck(c)) {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

Position Board::findStartSquare(char pieceType, bool isWhiteToMove, Position endPos, char fileDisambiguation, char rankDisambiguation) {
    std::vector<Position> candidates;

    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            Position currentPos(x, y);
            Piece* p = getPiece(currentPos);

            if (p != nullptr && p->getPieceType() == pieceType && p->isWhite() == isWhiteToMove) {

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
        return Position{-1, -1};
    } else {
        std::cerr << "Hiba: Nem található olyan bábu, amellyel megtehető ez a lépés!" << std::endl;
        return Position{-1, -1};
    }

}

void Board::clearBoard() {
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            if (board[x][y] != nullptr) {
                delete board[x][y];
                board[x][y] = nullptr;
            }
        }
    }
}

bool Board::loadFromFEN(const std::string& fen) {
    clearBoard();

    int x = 0;
    int y = 7;
    size_t i = 0;

    for (; i < fen.length(); i++) {
        char c = fen[i];

        if (c == ' ') {
            i++;
            break;
        }

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
                placePiece(p, Position(x,y));
                x++;
            }
        }
    }

    if (i < fen.length()) {
        turn = (fen[i] == 'w') ? Color::WHITE : Color::BLACK;
    }
    // TO IMPL: 
    // 1. en passant from FEN
    // 2. castling from FEN
    // 3. ...?

    return true;
}