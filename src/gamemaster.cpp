#include "gamemaster.hpp"

#include "pgnhandler.hpp"
#include "constants.hpp"
#include "logger.hpp"

#include <iostream>
#include <regex>
#include <limits>
#include <iomanip>
#include <fstream>

namespace
{

};

void GameMaster::gameLoop(PGNMetadata &metadata)
{

    std::string userInput;

    while (true)
    {
        // 1. take input in some form, maybe pgn, maybe a more intuitive way for the player (more code more annoyance)
        std::cin >> userInput;
        for (char &c : userInput)
            c = std::tolower(c);

        // 2. make into coords
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
        if (p && std::tolower(p->getPieceType()) == 'p')
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
        bool moveSuccessful = processMove(currentMoveStartPos, currentMoveEndPos, promotedTo);

        if (!moveSuccessful)
        {
            continue;
        }
        else
        {
            board.nextTurn();
        }
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

bool GameMaster::processMove(Position<> startPos, Position<> endPos, char promotedTo)
{
    const Piece *p = board.getPiece(startPos);

    if (!p || p->getColor() != board.getTurn())
    {
        std::cerr << "Helytelen lépés: Nem te vagy soron." << std::endl;
        return false;
    }

    const Piece *target = board.getPiece(endPos);

    // en passant
    bool isEnPassant = false;
    char pType = std::tolower(p->getPieceType());
    if (pType == 'p' && endPos == board.getEnPassantTarget())
    {
        if (std::abs(startPos.x - endPos.x) == 1)
        {
            isEnPassant = true;
        }
    }

    if (isEnPassant || p->isValidMove(startPos, endPos, target))
    {
        if (p->canJump() || board.isPathClear(startPos, endPos))
        {

            // castling
            bool isCastle = (p->isKing() && std::abs(startPos.x - endPos.x) == 2);
            int rookStartX = -1;
            int rookEndX = -1;

            if (isCastle)
            {
                // Nem lehet sakkból kilépni sáncolással
                if (board.isCheck(board.getTurn()))
                {
                    std::cerr << "Helytelen lépés: Nem lehet a sakkot sáncolással megszakítani." << std::endl;
                    return false;
                }

                // Bástya megkeresése
                bool kingside = (endPos.x > startPos.x);
                rookStartX = kingside ? 7 : 0; // h-file (7) or a-file (0)
                rookEndX = kingside ? 5 : 3;   // f-file (5) or d-file (3)

                Position<> rookStartPos(rookStartX, startPos.y);
                const Piece *rook = board.getPiece(rookStartPos);

                // 3. Megnézzük hogy a bástya ott van-e illetve, hogy lépett-e már
                if (!rook || rook->getColor() != board.getTurn() || rook->getPieceType() != 'R' || rook->getHasMoved())
                {
                    std::cerr << "Helytelen lépés: A bástya nincs a kezdőhelyén, vagy már egyszer elmozdult onnan." << std::endl;
                    return false;
                }

                // 4. Ha a királynő felé akarunk sáncolni akkor meg kell nézni a B oszlopot is
                if (!kingside)
                {
                    Position<> bSquare(1, startPos.y);
                    if (board.getPiece(bSquare) != nullptr)
                    {
                        std::cerr << "Helytelen lépés: Nem szabad a király és bástya között az út." << std::endl;
                        return false;
                    }
                }

                // 5. Nem lehet sáncolni ha a kezdő és végpont között sakkban lennénk
                int passX = kingside ? 5 : 3;
                Board passCheckBoard(board);
                passCheckBoard.movePiece(startPos, Position<>(passX, startPos.y));

                if (passCheckBoard.isCheck(board.getTurn()))
                {
                    std::cerr << "Helytelen lépés: Nem sáncolhatsz sakkon keresztül." << std::endl;
                    return false;
                }
            }

            // simulation
            Board clonedBoard(board);
            clonedBoard.movePiece(startPos, endPos);

            if (isEnPassant)
            {
                clonedBoard.removePiece(Position<>(endPos.x, startPos.y));
            }

            if (isCastle)
            {
                clonedBoard.movePiece(Position<>(rookStartX, startPos.y), Position<>(rookEndX, startPos.y));
            }

            if (clonedBoard.isCheck(board.getTurn()))
            {
                std::cerr << "Helytelen lépés: Nem léphetsz sakkba." << std::endl;
                return false;
            }

            Piece *capturedPiece = nullptr;
            if (isEnPassant)
            {
                capturedPiece = const_cast<Piece *>(board.getPiece(Position<>(endPos.x, startPos.y)));
            }
            else
            {
                capturedPiece = const_cast<Piece *>(target);
            }

            bool isCapture = (capturedPiece);
            char movedPieceChar = p->getPieceType();

            // real move

            if (isEnPassant)
            {
                board.removePiece(Position<>(endPos.x, startPos.y));
            }

            if (isCapture)
            {
                capturedPiece->getColor() == Color::WHITE ? pointsBlack += capturedPiece->getValue()
                                                          : pointsWhite += capturedPiece->getValue();
            }

            board.movePiece(startPos, endPos);

            if (promotedTo != '\0')
            {
                board.promotePiece(endPos, promotedTo, p->getColor());
            }

            Piece *movedPiece = board.getPiece(endPos);
            if (movedPiece)
            {
                movedPiece->setHasMoved();
            }

            if (isCastle)
            {
                Position<> rookEndPos(rookEndX, startPos.y);

                board.movePiece(Position<>(rookStartX, startPos.y), rookEndPos);

                Piece *movedRook = board.getPiece(rookEndPos);
                if (movedRook)
                {
                    movedRook->setHasMoved();
                }
            }

            Position<> nextEnPassantTarget{-1, -1};

            if (pType == 'p')
            {
                if (std::abs(startPos.y - endPos.y) == 2)
                {
                    int skippedY = (startPos.y + endPos.y) / 2;
                    nextEnPassantTarget = Position<>(startPos.x, skippedY);
                }
            }

            board.setEnPassantTarget(nextEnPassantTarget);

            Color opponentColor = (board.getTurn() == Color::WHITE) ? Color::BLACK : Color::WHITE;

            bool isCheckMove = board.isCheck(opponentColor);
            bool isCheckMateMove = board.isCheckMate(opponentColor);

            moveHistory.emplace_back(
                startPos,
                endPos,
                Move::Flags{
                    .isCapture = isCapture,
                    .isCastle = isCastle,
                    .isEnPassant = isEnPassant,
                    .isCheck = isCheckMove,
                    .isCheckMate = isCheckMateMove,
                },
                movedPieceChar,
                promotedTo,
                capturedPiece);

            return true;
        }
    }
    std::cerr << "Helytelen lépés: Ez a bábu nem tud így lépni." << std::endl;
    return false;
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
    bool validEnd   = (userInput[2] >= 'a' && userInput[2] <= 'h') && 
                      (userInput[3] >= '1' && userInput[3] <= '8');

    if (!validStart || !validEnd) return false;

    if (userInput.length() == 5) {
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
        replayPGN(pgnFilePath);
    }
}

void GameMaster::replayPGN(std::string_view pgnFilePath)
{
    auto parsed = PGNHandler::parseFile(pgnFilePath, board).value_or(std::make_pair(PGNMetadata{}, std::vector<Move>{}));
    auto metadata = parsed.first;
    this->moveHistory = parsed.second;
    std::string startingFen = metadata.fen.empty() ? std::string(DEFAULT_FEN) : metadata.fen;

    int currentMoveIndex = 0;
    std::vector<std::string> historyStates;

    board.loadFromFEN(startingFen);
    historyStates.push_back(board.generateFEN());

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
            const auto &m = moveHistory[currentMoveIndex];

            board.movePiece(m.startPos, m.endPos);
            if (m.flags.isCastle)
            {
                using namespace CASTLING_POSITION_CONSTANTS;
                if (m.endPos.x == KING_DEST_KINGSIDE_X)
                {
                    board.movePiece(Position<>(ROOK_START_KINGSIDE_X, m.startPos.y), Position<>(ROOK_DEST_KINGSIDE_X, m.startPos.y));
                }
                else if (m.endPos.x == KING_DEST_QUEENSIDE_X)
                {
                    board.movePiece(Position<>(ROOK_START_QUEENSIDE_X, m.startPos.y), Position<>(ROOK_DEST_QUEENSIDE_X, m.startPos.y));
                }
            }
            else if (m.flags.isEnPassant)
            {
                board.removePiece(Position<>(m.endPos.x, m.startPos.y));
            }
            else if (m.promotedTo != '\0')
            {
                board.promotePiece(m.endPos, m.promotedTo, board.getTurn());
            }
            board.nextTurn();

            currentMoveIndex++;
            if (static_cast<size_t>(currentMoveIndex) >= historyStates.size())
            {
                historyStates.push_back(board.generateFEN());
            }

            renderer.display(pointsWhite, pointsBlack);
        }
        else if (input == 'p' && currentMoveIndex > 0)
        {
            currentMoveIndex--;
            
            LOG_DEBUG("Loading FEN: " + historyStates[currentMoveIndex]);

            board.loadFromFEN(historyStates[currentMoveIndex]);

            renderer.display(pointsWhite, pointsBlack);
        }
    }
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

    PGNHandler::savePGN(metadata, moveHistory);
}