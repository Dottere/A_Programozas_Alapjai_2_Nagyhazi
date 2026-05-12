#include "board.hpp"

#include "piece.hpp"

#include <cmath>
#include <vector>
#include <iostream> 
#include <sstream>
#include <cctype>
#include <regex>
#include <charconv>


namespace {
    const std::regex FEN_REGEX(R"(^([prnbqkPRNBQK1-8]+(?:/[prnbqkPRNBQK1-8]+){7})\s+([wb])\s+(-|[KQkq]{1,4})\s+(-|[a-h][36])\s+(\d+)\s+([1-9]\d*)$)");
}

Board::Board(const Board& b) {
    for (int i = 0; i < BOARD_SIZE; i++){
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (b.board[i][j]) {
                this->board[i][j] = b.board[i][j]->clone();
            } else {
                this->board[i][j].reset();
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



bool Board::placePiece(Square piece, Position<> pos) {
    if (!isOnBoard(pos)) return false;

    // A tömb a bal felső sarokban kezdi az indexelést, míg a sakk szabályai a bal alsó sarokban
    auto arrayY = 7 - pos.y;

    board[pos.x][arrayY] = std::move(piece);
    return true;
}

std::optional<Position<>> Board::findKing(Color c) {
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            Piece* p = getPiece({x, y});
            if (p && p->isKing() && p->getColor() == c) {
                return Position<>{x, y};
            }
        }
    }
    return std::nullopt;
}

bool Board::isCheck(Color c) {
    Position<> kingPos = findKing(c).value_or(Position<>{-1,-1});
    if (!kingPos.isValid()) return false;

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            Position<> attackerPos(x, y);
            Piece* attacker = getPiece(attackerPos);

            if (attacker && 
                attacker->getColor() != c && 
                attacker->isValidMove(attackerPos, kingPos, getPiece(kingPos)) &&
                (attacker->canJump() || isPathClear(attackerPos, kingPos))) 
            {
                return true;
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

            if (!p || p->getColor() != c) continue;

            for (int endY = 7; endY >= 0; endY--) {
                for (int endX = 0; endX < 8; endX++) {
                    Position<> endPos(endX, endY);
                    Piece* target = getPiece(endPos);

                    if (target && target->getColor() == c) continue;

                    if (p->isValidMove(startPos, endPos, target) && 
                    (p->canJump() || isPathClear(startPos, endPos))) {
                        Board clonedBoard(*this);
                        clonedBoard.movePiece(startPos, endPos);
                        if (!clonedBoard.isCheck(c)) return true;
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

std::optional<Position<>> Board::findStartSquare(char pieceType, bool isWhiteToMove, Position<> endPos, char fileDisambiguation, char rankDisambiguation) {
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
        return std::nullopt;
    } else {
        return std::nullopt;
    }

}

void Board::clearBoard() {
    for (auto& row : board) {
        for (auto& square : row) {
            square.reset();
        }
    }

    turn = Color::WHITE;
    canWhiteCastleKingside = canWhiteCastleQueenside = false;
    canBlackCastleKingside = canBlackCastleQueenside = false;
    enPassantTarget.reset();
    halfMoveClock = 0;
    fullMoveNumber = 1;
    whiteCaptured.clear();
    blackCaptured.clear();
}

bool Board::loadFromFEN(std::string_view fen) {
    std::string fenStr(fen);
    std::smatch match;

    if (!std::regex_match(fenStr, match, FEN_REGEX)) return false;

    clearBoard();

    std::string_view placement   { &*match[1].first, static_cast<size_t>(match[1].length()) };
    std::string_view activeColor { &*match[2].first, static_cast<size_t>(match[2].length()) };
    std::string castling { &*match[3].first, static_cast<size_t>(match[3].length()) };
    std::string enPassant { &*match[4].first, static_cast<size_t>(match[4].length()) };

    int halfMoveClock = 0;
    int fullMoveNumber = 1;

    std::from_chars(&*match[5].first, &*match[5].second, halfMoveClock);
    std::from_chars(&*match[6].first, &*match[6].second, fullMoveNumber);

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
            
            Square Piece = nullptr;
            
            switch (type) {
                case 'r' : Piece = std::make_unique<Rook>(color); break;
                case 'n' : Piece = std::make_unique<Knight>(color); break;
                case 'b' : Piece = std::make_unique<Bishop>(color); break;
                case 'p' : Piece = std::make_unique<Pawn>(color); break;
                case 'q' : Piece = std::make_unique<Queen>(color); break;
                case 'k' : Piece = std::make_unique<King>(color); break;
            }

            if (Piece) {
                if (placePiece(std::move(Piece), Position<>(x, y))) x++;
                else return false;
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

std::string Board::generateFEN() const {
    std::string fen; //  A hivatalos FEN sztringek elméletben maximum 90 hosszúak lehetnek
    fen.reserve(100); //  Source: https://chess.stackexchange.com/questions/30004/longest-possible-fen

    for (int arrayY = 0; arrayY < BOARD_SIZE; ++arrayY) {
        int emptyCount = 0;

        for (const auto& column : board) {
            const Piece* p = column[arrayY].get();

            if (!p) {
                emptyCount++;
            } else {
                if (emptyCount > 0) {
                    fen += std::to_string(emptyCount);
                    emptyCount = 0;
                }
                char type = p->getPieceType();
                if (p->isWhite()) {
                    fen += static_cast<char>(std::toupper(static_cast<unsigned char>(type)));
                } else {
                    fen += static_cast<char>(std::tolower(static_cast<unsigned char>(type)));
                }
            }
        }

        if (emptyCount > 0) {
            fen += std::to_string(emptyCount);
        }

        if (arrayY < BOARD_SIZE - 1) {
            fen += '/';
        }
    
    }

    fen += (turn == Color::WHITE ? " w " : " b ");
   
    size_t castlingStart = fen.size();
    if (canWhiteCastleKingside)  { fen += 'K'; }
    if (canWhiteCastleQueenside) { fen += 'Q'; }
    if (canBlackCastleKingside)  { fen += 'k'; }
    if (canBlackCastleQueenside) { fen += 'q'; }
    
    if (fen.size() == castlingStart) {
        fen += '-';
    }

    fen += ' ';
    if (!enPassantTarget) {
        fen += '-';
    } else {
        fen += static_cast<char>('a' + enPassantTarget->x);
        fen += static_cast<char>('1' + enPassantTarget->y);
    }


    fen += ' ';
    fen += std::to_string(halfMoveClock);
    fen += ' ';
    fen += std::to_string(fullMoveNumber);

    return fen;
}