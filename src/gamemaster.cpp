#include "gamemaster.hpp"

#include <iostream>
#include <regex>

void GameMaster::gameLoop(PGNMetadata& metadata) {
    
        std::string userInput;
        renderer.display();

        while (true) {
            // 1. take input in some form, maybe pgn, maybe a more intuitive way for the player (more code more annoyance)
            std::cin >> userInput;
            for (char &c : userInput) c = std::tolower(c);

            // 2. make into coords
            Position<> currentMoveStartPos;
            Position<> currentMoveEndPos;
            if (!isValidInput(userInput)) {
                std::cerr << "Helytelen lépésformátum! Kérlek használd a kezdőmező célmező formátumot (Pl. e2e4)!" << std::endl;
                continue;
            }

            if (userInput == "o-o") {
                userInput = board.isWhiteToMove() ? "e1g1" : "e8g8";
            } else if (userInput == "o-o-o") {
                userInput = board.isWhiteToMove() ? "e1c1" : "e8c8";
            }

            currentMoveStartPos = { userInput[0]-'a', userInput[1]-'1' };
            currentMoveEndPos = { userInput[2]-'a', userInput[3]-'1' };
            // 3. processMove(startPos, endPos)

            bool moveSuccessful = processMove(currentMoveStartPos, currentMoveEndPos);

            if (!moveSuccessful) {
                continue;
            }
            // 4. change turn
            board.nextTurn();
            // 5. display
            renderer.display();

            // 6. game over
            if (board.isCheckMate(board.getTurn())) {
                std::string winner = board.isWhiteToMove() ? "Fekete" : "Fehér";
                std::cout << "Sakkmatt! " << winner << " nyert!" << std::endl;
                metadata.result = board.isWhiteToMove() ? "0-1" : "1-0";
                break;
            } else if (board.isStaleMate(board.getTurn())) {
                std::cout << "Patt! A játék döntetlen." << std::endl;
                metadata.result = "1/2-1/2";
                break;
            }
        }
}

bool GameMaster::processMove(Position<> startPos, Position<> endPos) {
        const Piece* p = board.getPiece(startPos);
        
        if (!p || p->getColor() != board.getTurn()) {
            std::cerr << "Helytelen lépés: Nem te vagy soron." << std::endl;
            return false;
        }

        const Piece* target = board.getPiece(endPos);
        
        //en passant
        bool isEnPassant = false;
        char pType = std::tolower(p->getPieceType());
        if (pType == 'p' && endPos == board.getEnPassantTarget()) {
            if (std::abs(startPos.x - endPos.x) == 1) {
                isEnPassant = true;
            }
        }

        if (isEnPassant || p->isValidMove(startPos, endPos, target)) {
            if (p->canJump() || board.isPathClear(startPos, endPos)) {

                //castling
                bool isCastle = (p->isKing() && std::abs(startPos.x - endPos.x) == 2);
                int rookStartX = -1;
                int rookEndX = -1;

                if (isCastle) {
                    // Nem lehet sakkból kilépni sáncolással
                    if (board.isCheck(board.getTurn())) {
                        std::cerr << "Helytelen lépés: Nem lehet a sakkot sáncolással megszakítani." << std::endl;
                        return false;
                    }

                    // Bástya megkeresése
                    bool kingside = (endPos.x > startPos.x);
                    rookStartX = kingside ? 7 : 0;        // h-file (7) or a-file (0)
                    rookEndX = kingside ? 5 : 3;          // f-file (5) or d-file (3)
                    
                    Position<> rookStartPos(rookStartX, startPos.y);
                    const Piece* rook = board.getPiece(rookStartPos);

                    // 3. Megnézzük hogy a bástya ott van-e illetve, hogy lépett-e már
                    if (!rook || rook->getColor() != board.getTurn() || rook->getPieceType() != 'R' || rook->getHasMoved()) {
                        std::cerr << "Helytelen lépés: A bástya nincs a kezdőhelyén, vagy már egyszer elmozdult onnan." << std::endl;
                        return false;
                    }

                    // 4. Ha a királynő felé akarunk sáncolni akkor meg kell nézni a B oszlopot is
                    if (!kingside) {
                        Position<> bSquare(1, startPos.y);
                        if (board.getPiece(bSquare) != nullptr) {
                            std::cerr << "Helytelen lépés: Nem szabad a király és bástya között az út." << std::endl;
                            return false;
                        }
                    }

                    // 5. Nem lehet sáncolni ha a kezdő és végpont között sakkban lennénk
                    int passX = kingside ? 5 : 3;
                    Board passCheckBoard(board);
                    passCheckBoard.movePiece(startPos, Position<>(passX, startPos.y));
                    
                    if (passCheckBoard.isCheck(board.getTurn())) {
                        std::cerr << "Helytelen lépés: Nem sáncolhatsz sakkon keresztül." << std::endl;
                        return false;
                    }
                }

                // simulation
                Board clonedBoard(board);
                clonedBoard.movePiece(startPos, endPos);

                if (isEnPassant) {
                    clonedBoard.removePiece(Position<>(endPos.x, startPos.y));
                }

                clonedBoard.movePiece(startPos, endPos);


                if (isCastle) {
                    clonedBoard.movePiece(Position<>(rookStartX, startPos.y), Position<>(rookEndX, startPos.y));
                }
                

                if (clonedBoard.isCheck(board.getTurn())) {
                    std::cerr << "Helytelen lépés: Nem léphetsz sakkba." << std::endl;
                    return false;
                }

                Piece* capturedPiece = nullptr;
                if (isEnPassant) {
                    capturedPiece = const_cast<Piece*>(board.getPiece(Position<>(endPos.x, startPos.y)));
                } else {
                    capturedPiece = const_cast<Piece*>(target);
                }

                bool isCapture = (capturedPiece != nullptr);
                char movedPieceChar = p->getPieceType();

                char promotedTo = '\0';


                // real move

                if (isEnPassant) {
                    board.removePiece(Position<>(endPos.x, startPos.y));
                }

                board.movePiece(startPos, endPos);

                Piece* movedPiece = board.getPiece(endPos);
                    if (movedPiece) {
                    movedPiece->setHasMoved();
                }

                if (isCastle) {
                    Position<> rookEndPos(rookEndX, startPos.y);

                    board.movePiece(Position<>(rookStartX, startPos.y), rookEndPos);

                    Piece* movedRook = board.getPiece(rookEndPos);
                    if (movedRook) {
                        movedRook->setHasMoved();
                    }
                }

                Position<> nextEnPassantTarget{-1, -1};

                if (pType == 'p') {
                    if (std::abs(startPos.y - endPos.y) == 2) {
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
                    isCapture,
                    isCastle,
                    isEnPassant,
                    isCheckMove,
                    isCheckMateMove,
                    movedPieceChar,
                    promotedTo,
                    capturedPiece
                );

                return true;
            }
        }
        std::cerr << "Helytelen lépés: Ez a bábu nem tud így lépni." << std::endl;
        return false;
}

// private
bool GameMaster::isValidInput(std::string userInput) {
    static const std::regex pattern("^([a-h][1-8][a-h][1-8]|o-o-o|0-0-0|o-o|0-0)$");
    return std::regex_match(userInput, pattern);
}