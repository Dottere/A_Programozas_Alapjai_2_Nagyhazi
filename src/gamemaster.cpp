#include "gamemaster.hpp"

#include "pgnhandler.hpp"
#include "constants.hpp"
#include "logger.hpp"
#include "exceptions.h"

#include <iostream>
#include <regex>
#include <limits>
#include <iomanip>
#include <fstream>

bool GameMaster::isValidActivePiece(const Piece *p) const
{
    if (!p || p->getColor() != board.getTurn())
    {
        std::cerr << "Helytelen lépés: Nem te vagy soron, vagy üres mezőt választottál." << std::endl;
        return false;
    }

    return true;
}
bool GameMaster::isEnPassantMove(const Piece *p, Position<> startPos, Position<> endPos) const
{
    if (p->isPromotionEligible(endPos) && endPos == board.getEnPassantTarget())
    {
        return std::abs(startPos.x - endPos.x) == 1;
    }
    return false;
}
bool GameMaster::validateCastlingRules(Position<> startPos, Position<> endPos, int &rookStartX, int &rookEndX) const
{
    if (board.isCheck(board.getTurn()))
    {
        std::cerr << "Helytelen lépés: Nem lehet a sakkot sáncolással megszakítani." << std::endl;
        return false;
    }

    bool kingside = (endPos.x > startPos.x);
    rookStartX = kingside ? 7 : 0; // h-file (7) or a-file (0)
    rookEndX = kingside ? 5 : 3;   // f-file (5) or d-file (3)

    const Piece *rook = board.getPiece(Position<>(rookStartX, startPos.y));

    if (!rook || rook->getColor() != board.getTurn() || rook->getPieceType() != 'R' || rook->getHasMoved())
    {
        std::cerr << "Helytelen lépés: A bástya nincs a kezdőhelyén, vagy már egyszer elmozdult onnan." << std::endl;
        return false;
    }

    if (!kingside && board.getPiece(Position<>(1, startPos.y)) != nullptr)
    {
        std::cerr << "Helytelen lépés: Nem szabad a király és bástya között az út." << std::endl;
        return false;
    }

    int passX = kingside ? 5 : 3;
    Board passCheckBoard(board);
    passCheckBoard.makeMove(startPos, Position<>(passX, startPos.y));

    if (passCheckBoard.isCheck(board.getTurn()))
    {
        std::cerr << "Helytelen lépés: Nem sáncolhatsz sakkon keresztül." << std::endl;
        return false;
    }

    return true;
}
void GameMaster::executeBoardMovement(const Move &move, int rookStartX, int rookEndX)
{
    if (move.flags.isEnPassant)
    {
        board.removePiece(Position<>(move.endPos.x, move.startPos.y));
    }

    board.makeMove(move.startPos, move.endPos);

    if (move.promotedTo != '\0')
    {
        Piece *movedPiece = board.getPiece(move.endPos);
        board.promotePiece(move.endPos, move.promotedTo, movedPiece->getColor());
    }

    Piece *movedPiece = board.getPiece(move.endPos);
    if (movedPiece)
    {
        movedPiece->setHasMoved(true);
    }

    if (move.flags.isCastle)
    {
        Position<> rookEndPos(rookEndX, move.startPos.y);
        board.makeMove(Position<>(rookStartX, move.startPos.y), rookEndPos);
        Piece *movedRook = board.getPiece(rookEndPos);
        if (movedRook)
        {
            movedRook->setHasMoved(true);
        }
    }
}
void GameMaster::rollbackInvalidMove(const Move &move)
{
    board.undoMove(move);
    board.nextTurn();
    std::cerr << "Helytelen lépés: Nem léphetsz sakkba" << std::endl;
}
void GameMaster::finalizeMove(Move &currentMove, int pointsGained)
{
    if (currentMove.flags.isCapture && currentMove.capturedPieceType)
    {
        if (currentMove.capturedPieceColor == Color::WHITE)
        {
            pointsBlack += pointsGained;
        }
        else
        {
            pointsWhite += pointsGained;
        }
    }

    Position<> nextEnPassantTarget{-1, -1};
    if (std::tolower(currentMove.movedPiece) == 'p' && std::abs(currentMove.startPos.y - currentMove.endPos.y) == 2)
    {
        int skippedY = (currentMove.startPos.y + currentMove.endPos.y) / 2;
        nextEnPassantTarget = Position<>(currentMove.startPos.x, skippedY);
    }
    board.setEnPassantTarget(nextEnPassantTarget);

    Color opponentColor = (board.getTurn() == Color::WHITE) ? Color::BLACK : Color::WHITE;
    currentMove.flags.isCheck = board.isCheck(opponentColor);
    currentMove.flags.isCheckMate = board.isCheckMate(opponentColor);

    moveHistory.push_back(currentMove);
}

Move GameMaster::createMoveRecord(const Piece *p, Position<> startPos, Position<> endPos, char promotedTo, bool isEnPassant, bool isCastle, Piece *capturedPiece) const
{
    bool isCapture = (capturedPiece != nullptr);
    Position<> prevEP = board.getEnPassantTarget().value_or(Position<>{-1, -1});

    return Move(
        startPos, endPos,
        Move::Flags{
            !p->getHasMoved(), // wasFirstMove
            isCapture,
            isCastle,
            isEnPassant,
            false, // isCheck
            false, // isCheckMate
            {0} // reserved for bit packing
        },
        p->getPieceType(), promotedTo, capturedPiece, prevEP, {0,0,0,0,0});
}

void GameMaster::gameLoop(PGNMetadata &metadata)
{

    std::string userInput;

    while (true)
    {
        std::cin >> userInput;
        for (char &c : userInput)
            c = std::tolower(c);

        Position<> currentMoveStartPos;
        Position<> currentMoveEndPos;
        if (!isValidInput(userInput))
        {
            std::cerr << "Helytelen lépésformátum! Kérlek használd a kezdőmező célmező formátumot (Pl. e2e4)!" << std::endl;
            continue;
        }

        if (userInput == "forfeit" || userInput == "ff")
        {
            std::cout << "Biztosan fel akarod adni? (i/n): ";
            char ans;
            std::cin >> ans;
            if (std::tolower(ans) != 'i')
                continue;

            std::string winner = board.isWhiteToMove() ? "Fehér" : "Sötét";
            std::cout << winner << " feladta." << std::endl;
            metadata.result = board.isWhiteToMove() ? "0-1" : "1-0";
            break;
        }

        if (userInput == "draw")
        {
            board.nextTurn();
            renderer.display(pointsWhite, pointsBlack);
            std::cout << "Elfogadod a döntetlent? (i/n): ";
            char ans;
            std::cin >> ans;
            if (std::tolower(ans) == 'i')
            {
                std::cout << "A játék döntetlen." << std::endl;
                metadata.result = "1/2-1/2";
                break;
            }
            else if (std::tolower(ans) == 'n')
            {
                board.nextTurn();
                continue;
            }
            else
            {
                continue;
            }
        }

        if (userInput == "o-o" || userInput == "0-0")
        {
            userInput = board.isWhiteToMove() ? "e1g1" : "e8g8";
        }
        else if (userInput == "o-o-o" || userInput == "0-0-0")
        {
            userInput = board.isWhiteToMove() ? "e1c1" : "e8c8";
        }

        currentMoveStartPos = {userInput[0] - 'a', userInput[1] - '1'};
        currentMoveEndPos = {userInput[2] - 'a', userInput[3] - '1'};

        char promotedTo = '\0';

        const Piece *p = board.getPiece(currentMoveStartPos);
        if (p && p->isPromotionEligible(currentMoveEndPos))
        {
            if (currentMoveEndPos.y == 0 || currentMoveEndPos.y == 7)
            {
                if (userInput.length() == 5)
                {
                    promotedTo = userInput[4];
                }
                else
                {
                    std::cout << "Mire szeretnéd cserélni a gyalogot? (q/r/b/n): ";
                    std::cin >> promotedTo;
                    promotedTo = std::tolower(promotedTo);

                    while (promotedTo != 'q' && promotedTo != 'r' && promotedTo != 'b' && promotedTo != 'n')
                    {
                        std::cout << "Érvénytelen bábu! Kérlek válassz ezek közül (q/r/b/n): ";
                        std::cin >> promotedTo;
                        promotedTo = std::tolower(promotedTo);
                    }
                }
            }
        }

        try {processMove(currentMoveStartPos, currentMoveEndPos, promotedTo); }
        catch (const ChessExcept::InvalidMoveException& e)
        {
            std::cerr << e.what() << '\n';
            continue;
        }

        board.nextTurn();
        
        renderer.display(pointsWhite, pointsBlack);

        // 6. game over
        if (board.isCheckMate(board.getTurn()))
        {
            std::string winner = board.isWhiteToMove() ? "Sötét" : "Fehér";
            std::cout << "Sakkmatt! " << winner << " nyert!" << std::endl;
            metadata.result = board.isWhiteToMove() ? "0-1" : "1-0";
            break;
        }
        else if (board.isStaleMate(board.getTurn()))
        {
            std::cout << "Patt! A játék döntetlen." << std::endl;
            metadata.result = "1/2-1/2";
            break;
        }
    }
}

void GameMaster::processMove(Position<> startPos, Position<> endPos, char promotedTo)
{
    const Piece *p = board.getPiece(startPos);

    if (!isValidActivePiece(p))
        throw ChessExcept::NonexistentPiecePointerException("Starting position is invalid! No piece stands there.");

    bool isEnPassant = isEnPassantMove(p, startPos, endPos);

    if (!isEnPassant && !p->isValidMove(startPos, endPos, board.getPiece(endPos)))
    {
        throw ChessExcept::InvalidMoveException("Helytelen lépés: Ez a bábu nem tud így lépni.");
    }

    auto path = p->getPath(startPos, endPos);

    if (!board.isPathClear(path))
    {
        throw ChessExcept::InvalidMoveException("Helytelen lépés: Útban van egy bábu.");
    }

    int rookStartX = -1, rookEndX = -1;
    bool isCastle = ((startPos == board.getWhiteKingPos() || startPos == board.getBlackKingPos())
    && (std::abs(startPos.x - endPos.x) == 2));

    if (isCastle && !validateCastlingRules(startPos, endPos, rookStartX, rookEndX))
    {
        throw ChessExcept::InvalidCastlingException("Nem tudsz így sáncolni!");
    }

    Piece *capturedPiece = isEnPassant ? board.getPiece(Position<>(endPos.x, startPos.y)) : board.getPiece(endPos);

    int pointsGained = capturedPiece ? capturedPiece->getValue() : 0;

    Move currentMove = createMoveRecord(p, startPos, endPos, promotedTo, isEnPassant, isCastle, capturedPiece);

    executeBoardMovement(currentMove, rookStartX, rookEndX);

    if (board.isCheck(board.getTurn()))
    {
        rollbackInvalidMove(currentMove);
        throw ChessExcept::InCheckException("Nem léphetsz sakkba!");
    }

    finalizeMove(currentMove, pointsGained);
}

bool GameMaster::isValidInput(std::string_view userInput) const
{
    if (userInput == "forfeit" || userInput == "ff" || userInput == "draw" ||
        userInput == "o-o" || userInput == "0-0" ||
        userInput == "o-o-o" || userInput == "0-0-0")
        return true;

    if (userInput.length() < 4 || userInput.length() > 5)
        return false;

    bool validStart = (userInput[0] >= 'a' && userInput[0] <= 'h') &&
                      (userInput[1] >= '1' && userInput[1] <= '8');
    bool validEnd = (userInput[2] >= 'a' && userInput[2] <= 'h') &&
                    (userInput[3] >= '1' && userInput[3] <= '8');

    if (!validStart || !validEnd)
        return false;

    if (userInput.length() == 5)
    {
        char p = static_cast<char>(std::tolower(userInput[4]));
        return p == 'q' || p == 'r' || p == 'b' || p == 'n';
    }

    return true;
}

void GameMaster::run(std::string_view fenStr, std::string_view pgnFilePath)
{
    if (pgnFilePath.empty())
    {
        manualPlay(fenStr);
    }
    else
    {
        if(!replayPGN(pgnFilePath)) std::cerr << "Hiba: A kért fájl nem létezik!" << std::endl;
    }
}

bool GameMaster::replayPGN(std::string_view pgnFilePath)
{   
    
    std::string startingFen;

    try 
    {
        auto [metadata, moveHistory] = PGNHandler::parseFile(pgnFilePath, board);

        startingFen = metadata.fen.empty() ? std::string(DEFAULT_FEN) : metadata.fen;
        this->moveHistory = moveHistory;
    }
    catch (const ChessExcept::MissingResourceException& e) 
    {
        std::cerr << e.what() << '\n';
    }

    if (this->moveHistory.empty()) 
    {
        return false;
    }

    int currentMoveIndex = 0;

    board.loadFromFEN(startingFen);

    renderer.display(pointsWhite, pointsBlack);

    bool replaying = true;
    while (replaying)
    {
        std::cout << "Press 'n' (NEXT), 'p' (PREVIOUS), 'q' (QUIT): ";
        char input;
        std::cin >> input;

        if (input == 'q')
        {
            replaying = false;
        }
        else if (input == 'n' && static_cast<size_t>(currentMoveIndex) < moveHistory.size())
        {
            board.applyMove(moveHistory[currentMoveIndex]);
            currentMoveIndex++;
            renderer.display(pointsWhite, pointsBlack);
        }
        else if (input == 'p' && currentMoveIndex > 0)
        {
            currentMoveIndex--;
            board.undoMove(moveHistory[currentMoveIndex]);
            renderer.display(pointsWhite, pointsBlack);
        }
    }
    return true;
}

void GameMaster::manualPlay(std::string_view fenStr)
{
    PGNMetadata metadata;

    if (fenStr != std::string(DEFAULT_FEN))
    {
        metadata.setup = 1;
        metadata.fen = fenStr;
    }

    renderer.display(pointsWhite, pointsBlack);

    gameLoop(metadata);

    try { PGNHandler::savePGN(metadata, moveHistory); }
    catch (const ChessExcept::MissingResourceException& e)
    {
        std::cerr << e.what();
    }
}