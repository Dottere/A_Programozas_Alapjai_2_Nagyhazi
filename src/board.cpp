#include "board.hpp"

#include "piece.hpp"

#include <cmath>
#include <vector>
#include <cctype>
#include <regex>
#include <charconv>

namespace
{
    const std::regex FEN_REGEX(R"(^([prnbqkPRNBQK1-8]+(?:/[prnbqkPRNBQK1-8]+){7})\s+([wb])\s+(-|[KQkq]{1,4})\s+(-|[a-h][36])\s+(\d+)\s+([1-9]\d*)$)");

    std::optional<Position<>> findKing(const Board &b, Color c)
    {
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            for (int y = 0; y < BOARD_SIZE; y++)
            {
                const Piece *p = b.getPiece({x, y});
                if ((Position<>{x,y} == b.getWhiteKingPos() || Position<>{x,y} == b.getBlackKingPos())&& p && p->getColor() == c)
                {
                    return Position<>{x, y};
                }
            }
        }
        return std::nullopt;
    }
}

Board::Board(const Board &b)
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            if (b.board[i][j])
            {
                this->board[i][j] = b.board[i][j]->clone();
            }
            else
            {
                this->board[i][j].reset();
            }
        }
    }

    this->turn = b.turn;

    this->whiteCaptured.clear();
    this->blackCaptured.clear();

    this->castlingRight.whiteKingside = b.castlingRight.whiteKingside;
    this->castlingRight.whiteQueenside = b.castlingRight.whiteQueenside;
    this->castlingRight.blackKingside = b.castlingRight.blackKingside;
    this->castlingRight.blackQueenside = b.castlingRight.blackQueenside;

    this->enPassantTarget = b.enPassantTarget;

    this->halfMoveClock = b.halfMoveClock;
    this->fullMoveNumber = b.fullMoveNumber;
}

bool Board::isPathClear(const std::vector<Position<>>& path) const
{
    for (const auto& pos : path) 
    {
        if (getPiece(pos))
        {
            return false;
        } 
    }

    return true;
}

bool Board::placePiece(Square piece, Position<> pos)
{
    if (!isOnBoard(pos))
        return false;;

    at(pos) = std::move(piece);
    return true;
}

bool Board::isCheck(Color c)
{
    Position<> kingPos = findKing(*this, c).value_or(Position<>{-1, -1});
    if (!kingPos.isValid())
        return false;

    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            Position<> attackerPos(x, y);
            Piece *attacker = getPiece(attackerPos);

            if (attacker && attacker->getColor() != c)
            {
                if (attackerPos == whiteKingPos || attackerPos == blackKingPos)
                {
                    if (std::abs(attackerPos.x - kingPos.x) > 1 || std::abs(attackerPos.y - kingPos.y) > 1)
                    {
                        continue;
                    }
                }

                if (attacker->isValidMove(attackerPos, kingPos, getPiece(kingPos)) &&
                    isPathClear(attacker->getPath(attackerPos, kingPos)))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Board::hasLegalMoves(Color c)
{
    for (int startY = 7; startY >= 0; startY--)
    {
        for (int startX = 0; startX < 8; startX++)
        {
            Position<> startPos(startX, startY);
            Piece *p = getPiece(startPos);

            if (!p || p->getColor() != c)
                continue;

            for (int endY = 7; endY >= 0; endY--)
            {
                for (int endX = 0; endX < 8; endX++)
                {
                    Position<> endPos(endX, endY);
                    Piece *target = getPiece(endPos);

                    if (target && target->getColor() == c)
                        continue;

                    if (p->isValidMove(startPos, endPos, target)) 
                    {
                        auto path = p->getPath(startPos, endPos);
                        
                        if (isPathClear(path)) 
                        {                        
                        bool isCapture = (target != nullptr);
                        Color capturedColor = isCapture ? target->getColor() : Color::WHITE;
                        
                        makeMove(startPos, endPos);
                        
                        bool inCheck = isCheck(c);
                        
                        at(startPos) = std::move(at(endPos)); // Visszacsúsztatjuk a bábut a kiindulópontra
                        
                        if (whiteKingPos == endPos) whiteKingPos = startPos;
                        else if (blackKingPos == endPos) blackKingPos = startPos;
                        
                        if (isCapture)
                        {
                            if (capturedColor == Color::WHITE) {
                                at(endPos) = std::move(whiteCaptured.back());
                                whiteCaptured.pop_back();
                            } else {
                                at(endPos) = std::move(blackCaptured.back());
                                blackCaptured.pop_back();
                            }
                        }
                        
                        if (!inCheck)
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool Board::isCheckMate(Color c)
{
    return isCheck(c) && !hasLegalMoves(c);
}

bool Board::isStaleMate(Color c)
{
    return !isCheck(c) && !hasLegalMoves(c);
}

std::optional<Position<>> Board::findStartSquare(char pieceType, bool isWhiteToMove, Position<> endPos, char fileDisambiguation, char rankDisambiguation)
{
    std::vector<Position<>> candidates;

    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            Position<> currentPos(x, y);
            Piece *p = getPiece(currentPos);

            if (p && p->getPieceType() == pieceType && p->isWhite() == isWhiteToMove)
            {

                if (fileDisambiguation != '\0' && (x + 'a') != fileDisambiguation)
                    continue;
                if (rankDisambiguation != '\0' && (y + '1') != rankDisambiguation)
                    continue;

                if (p->isValidMove(currentPos, endPos, getPiece(endPos)))
                {   
                    if (isPathClear(p->getPath(currentPos, endPos)))
                    {
                        candidates.push_back(currentPos);
                    }
                }
            }
        }
    }

    if (candidates.size() == 1)
    {
        return candidates[0];
    }
    else if (candidates.size() > 1)
    {
        return std::nullopt;
    }
    else
    {
        return std::nullopt;
    }
}

void Board::clearBoard()
{
    for (auto &row : board)
    {
        for (auto &square : row)
        {
            square.reset();
        }
    }

    turn = Color::WHITE;
    
    castlingRight.whiteKingside = castlingRight.whiteQueenside = castlingRight.blackKingside = castlingRight.blackQueenside = {0};
    castlingRight.reserved = {0};

    enPassantTarget.reset();
    halfMoveClock = 0;
    fullMoveNumber = 1;
    whiteCaptured.clear();
    blackCaptured.clear();
    whiteKingPos = Position<>{-1, -1};
    blackKingPos = Position<>{-1, -1};
}

bool Board::loadFromFEN(std::string_view fen)
{
    std::string fenStr(fen);
    std::smatch match;

    if (!std::regex_match(fenStr, match, FEN_REGEX))
        return false;

    clearBoard();

    std::string_view placement{&*match[1].first, static_cast<size_t>(match[1].length())};
    std::string_view activeColor{&*match[2].first, static_cast<size_t>(match[2].length())};
    std::string castling{&*match[3].first, static_cast<size_t>(match[3].length())};
    std::string enPassant{&*match[4].first, static_cast<size_t>(match[4].length())};

    int halfMoveClock = 0;
    int fullMoveNumber = 1;

    std::from_chars(&*match[5].first, &*match[5].second, halfMoveClock);
    std::from_chars(&*match[6].first, &*match[6].second, fullMoveNumber);

    int x = 0;
    int y = 7;

    // Bábuk elhelyezése
    for (char c : placement)
    {

        if (c == '/')
        {
            y--;
            x = 0;
        }
        else if (std::isdigit(c))
        {
            x += (c - '0');
        }
        else
        {
            Color color = std::isupper(c) ? Color::WHITE : Color::BLACK;
            char type = std::tolower(c);

            Square Piece = nullptr;

            switch (type)
            {
            case 'r':
                Piece = std::make_unique<Rook>(color);
                break;
            case 'n':
                Piece = std::make_unique<Knight>(color);
                break;
            case 'b':
                Piece = std::make_unique<Bishop>(color);
                break;
            case 'p':
                Piece = std::make_unique<Pawn>(color);
                break;
            case 'q':
                Piece = std::make_unique<Queen>(color);
                break;
            case 'k':
                Piece = std::make_unique<King>(color);
                if (color == Color::WHITE) whiteKingPos = Position<>(x, y);
                else blackKingPos = Position<>(x, y);
                break;
            }

            if (Piece)
            {
                if (placePiece(std::move(Piece), Position<>(x, y)))
                    x++;
                else
                    return false;
            }
        }
    }

    // Soron következő játékos beállítása
    if (!activeColor.empty())
    {
        turn = (activeColor[0] == 'w') ? Color::WHITE : Color::BLACK;
    }

    // Sáncolási jogok
    if (castling != "-")
    {
        for (char c : castling)
        {
            switch (c)
            {
            case 'K':
                castlingRight.whiteKingside = true;
                break;
            case 'Q':
                castlingRight.whiteQueenside = true;
                break;
            case 'k':
                castlingRight.blackKingside = true;
                break;
            case 'q':
                castlingRight.blackQueenside = true;
                break;
            }
        }
    }

    // En Passant
    if (enPassant != "-" && enPassant.length() == 2)
    {
        int epX = enPassant[0] - 'a';
        int epY = enPassant[1] - '1';
        enPassantTarget = Position<>{epX, epY};
    }

    this->fullMoveNumber = fullMoveNumber;
    this->halfMoveClock = halfMoveClock;

    return true;
}

std::string Board::generateFEN() const
{
    std::string fen;  //  A hivatalos FEN sztringek elméletben maximum 90 hosszúak lehetnek
    fen.reserve(100); //  Source: https://chess.stackexchange.com/questions/30004/longest-possible-fen

    for (int arrayY = 0; arrayY < BOARD_SIZE; ++arrayY)
    {
        int emptyCount = 0;

        for (const auto &column : board)
        {
            const Piece *p = column[arrayY].get();

            if (!p)
            {
                emptyCount++;
            }
            else
            {
                if (emptyCount > 0)
                {
                    fen += std::to_string(emptyCount);
                    emptyCount = 0;
                }
                char type = p->getPieceType();
                if (p->isWhite())
                {
                    fen += static_cast<char>(std::toupper(static_cast<unsigned char>(type)));
                }
                else
                {
                    fen += static_cast<char>(std::tolower(static_cast<unsigned char>(type)));
                }
            }
        }

        if (emptyCount > 0)
        {
            fen += std::to_string(emptyCount);
        }

        if (arrayY < BOARD_SIZE - 1)
        {
            fen += '/';
        }
    }

    fen += (turn == Color::WHITE ? " w " : " b ");

    size_t castlingStart = fen.size();
    if (castlingRight.whiteKingside)
    {
        fen += 'K';
    }
    if (castlingRight.whiteQueenside)
    {
        fen += 'Q';
    }
    if (castlingRight.blackKingside)
    {
        fen += 'k';
    }
    if (castlingRight.blackQueenside)
    {
        fen += 'q';
    }

    if (fen.size() == castlingStart)
    {
        fen += '-';
    }

    fen += ' ';
    if (!enPassantTarget)
    {
        fen += '-';
    }
    else
    {
        fen += static_cast<char>('a' + enPassantTarget->x);
        fen += static_cast<char>('1' + enPassantTarget->y);
    }

    fen += ' ';
    fen += std::to_string(halfMoveClock);
    fen += ' ';
    fen += std::to_string(fullMoveNumber);

    return fen;
}

void Board::applyMove(const Move& m)
    {
        if (m.flags.isEnPassant) {
            removePiece(Position<>(m.endPos.x, m.startPos.y));
        }

        makeMove(m.startPos, m.endPos);

        if (m.promotedTo != '\0') {
            promotePiece(m.endPos, m.promotedTo, getTurn());
        }

        Piece *movedPiece = getPiece(m.endPos);
        if (movedPiece) {
            movedPiece->setHasMoved(true);
        }

        if (m.flags.isCastle) {
            bool kingside = (m.endPos.x > m.startPos.x);
            int rookStartX = kingside ? 7 : 0;
            int rookDestX = kingside ? 5 : 3;

            makeMove(Position<>(rookStartX, m.startPos.y), Position<>(rookDestX, m.startPos.y));
            Piece *movedRook = getPiece(Position<>(rookDestX, m.startPos.y));
            if (movedRook) movedRook->setHasMoved(true);
        }

        Position<> nextEnPassantTarget{-1, -1};
        if (std::tolower(m.movedPiece) == 'p' && std::abs(m.startPos.y - m.endPos.y) == 2) {
            nextEnPassantTarget = Position<>(m.startPos.x, (m.startPos.y + m.endPos.y) / 2);
        }
        setEnPassantTarget(nextEnPassantTarget);

        nextTurn();
    }

void Board::undoMove(const Move& m)
{
        if (whiteKingPos == m.endPos && m.movedPiece == 'K') whiteKingPos = m.startPos;
        else if (blackKingPos == m.endPos && m.movedPiece == 'K') blackKingPos = m.startPos;
        
        nextTurn();

        makeMove(m.endPos, m.startPos);

        if (m.promotedTo != '\0')
        {
            at(m.startPos) = std::make_unique<Pawn>(getPiece(m.startPos)->getColor());
        }

        if (m.flags.isCastle)
        {
            bool kingside = (m.endPos.x > m.startPos.x);
            int rookDestX = kingside ? 5 : 3;
            int rookStartX = kingside ? 7 : 0;

            makeMove(Position<>{rookDestX, m.startPos.y}, Position<>{rookStartX, m.startPos.y});

            at(m.startPos)->setHasMoved(false);
            at(Position<>(rookStartX, m.startPos.y))->setHasMoved(false);
        }
        else
        {
            if (m.flags.wasFirstMove) at(m.startPos)->setHasMoved(false);
        }

        if (m.flags.isCapture) 
        {
            Square revivedPiece;

            if (m.capturedPieceColor == Color::WHITE) 
            {
                revivedPiece = std::move(whiteCaptured.back());
                whiteCaptured.pop_back();
            }
            else
            {
                revivedPiece = std::move(blackCaptured.back());
                blackCaptured.pop_back();
            }

            if (m.flags.isEnPassant) 
            {
                Position<> epPawnPos{m.endPos.x, m.startPos.y};
                if (!placePiece(std::move(revivedPiece), epPawnPos)) {return;}
            }
            else
            {   
                if(placePiece(std::move(revivedPiece), m.endPos)) {return;}
            }
        }
        
        enPassantTarget = m.prevEnPassantTarget;
        castlingRight = m.prevCastlingRights;
    }

void Board::promotePiece(Position<> pos, char promotedTo, Color pieceColor)
{
        if (!isOnBoard(pos))
            return;
        Square &sq = at(pos);

        if (!sq)
            return;

        switch (std::tolower(static_cast<unsigned char>(promotedTo)))
        {
        case 'q':
            sq = std::make_unique<Queen>(pieceColor);
            break;
        case 'r':
            sq = std::make_unique<Rook>(pieceColor);
            break;
        case 'b':
            sq = std::make_unique<Bishop>(pieceColor);
            break;
        case 'n':
            sq = std::make_unique<Knight>(pieceColor);
            break;
        }
    }

void Board::removePiece(Position<> pos)
    {
        if (!isOnBoard(pos))
            return;

        Square &sq = at(pos);

        if (sq)
        {
            if (sq->isWhite())
                whiteCaptured.push_back(std::move(sq));
            else
                blackCaptured.push_back(std::move(sq));
        }
        sq.reset();
    }

void Board::makeMove(Position<> startPos, Position<> endPos)
{
        if (!isOnBoard(startPos) || !isOnBoard(endPos))
            return;

        if (startPos == whiteKingPos) whiteKingPos = endPos;
        else if (startPos == blackKingPos) blackKingPos = endPos;


        Square &startSquare = at(startPos);
        Square &endSquare = at(endPos);

        if (!startSquare)
            return;

        if (auto &target = endSquare)
        {
            if (target->isWhite())
            {
                whiteCaptured.push_back(std::move(target));
            }
            else
            {
                blackCaptured.push_back(std::move(target));
            }
        }

        endSquare = std::move(startSquare);
}