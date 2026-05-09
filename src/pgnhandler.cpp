#include "pgnhandler.hpp"
#include "gamemaster.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <regex>



std::vector<Move> PGNHandler::parseFile(std::string filePath, Board& board) {
    std::vector<Move> parsedMoves;
    std::ifstream file(filePath);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Hiba: Nem lehetett megnyitni a(z) " << filePath << " elérési úton lévő fájlt!" << std::endl;
        return parsedMoves;
    }

    std::string rawMoveText = "";

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        if (line[0] == '[') continue; // skip pgn metadata

        rawMoveText += line + " ";
    }
    file.close();

    rawMoveText = std::regex_replace(rawMoveText, std::regex(R"(\{.*?\})"), ""); // remove comments
    rawMoveText = std::regex_replace(rawMoveText, std::regex(R"(\(.*?\))"), ""); // remove variations
    rawMoveText = std::regex_replace(rawMoveText, std::regex(R"(\b\d+\.+ *)"), ""); // remove move numbers

    /**
     * @brief Tokenizálja a fennmaradó megtisztított SAN kifejezést (standard algebraic notation)
     * 
     * Megkeresi a bábukat [KQRBN], oszlopokat [a-h], sorokat [1-8], elfogásokat (x), sakkokat (+), mattot (#), 
     * és a sáncolást (O-O vagy O-O-O)
     */
    std::regex sanRegex(R"([KQRBN]?[a-h]?[1-8]?x?[a-h][1-8](?:=[QRBN])?[\+#]?|O-O(?:-O)?)");

    auto words_begin = std::sregex_iterator(rawMoveText.begin(), rawMoveText.end(), sanRegex);
    auto words_end = std::sregex_iterator();

    bool isWhiteToMove = true;

    for (std::sregex_iterator i = words_begin; i != words_end; i++) {
        std::string sanMove = i->str();

        // 1. make move object from san repr
        Move currentMove = sanToMoveObj(sanMove, board, isWhiteToMove);

        if (currentMove.startPos.x == -1) {
            std::cerr << "Hiba: Hibás PGN lépés: " << sanMove << ". Lehet, hogy hibás a PGN?" << std::endl;
            break;
        }

        // 2. make the move on the board
        board.movePiece(currentMove.startPos, currentMove.endPos);


        if (currentMove.isCastle) {
            if (currentMove.endPos.x == 6) {
                board.placePiece(std::unique_ptr<Piece>(board.getPiece(Position<>(7, currentMove.startPos.y))), 
                Position<>(5, currentMove.startPos.y));

                board.placePiece(nullptr, Position<>(7, currentMove.startPos.y));
            } else {
                board.placePiece(std::unique_ptr<Piece>(board.getPiece(Position<>(0, currentMove.startPos.y))), 
                Position<>(3, currentMove.startPos.y));

                board.placePiece(nullptr, Position<>(0, currentMove.startPos.y));
            }
        }

        // 3. save to parsed moves vector
        parsedMoves.push_back(currentMove);

        isWhiteToMove = !isWhiteToMove;
    }

    return parsedMoves;
}

std::string generatePGN(const std::vector<Move>& history) {
    std::string PGNString = "";

    // impl tbd

    return PGNString;
}

Move PGNHandler::sanToMoveObj(std::string sanMove, Board& board, bool isWhiteToMove) {
    bool isCapture = false;
    bool isCastle = false;
    bool isEnPassant = false;
    bool isCheck = false;

    char movedPiece = 'P';
    char promotedTo = '\0';

    Position<> startPos{-1,-1};
    Position<> endPos{-1,-1};
    Piece* capturedPiecePtr = nullptr;

    if (sanMove.back() == '+' || sanMove.back() == '#') {
        isCheck = true;
        sanMove.pop_back();
    }

    if (sanMove == "O-O" || sanMove == "O-O-O") {
        isCastle = true;
        movedPiece = 'K';
        
        int rank = isWhiteToMove ? 0 : 7;
        startPos = Position<>(4, rank);

        if (sanMove == "O-O") {
            endPos = Position<>(6, rank); // kingside
        } else {
            endPos = Position<>(2, rank); // queenside
        }

        return Move(startPos, endPos, isCapture, isCastle, isEnPassant, isCheck, movedPiece, promotedTo, nullptr);
    }

    size_t equalPos = sanMove.find('=');
    if (equalPos != std::string::npos) {
        promotedTo = sanMove.back();
        sanMove = sanMove.substr(0, equalPos);
    }

    size_t xPos = sanMove.find('x');
    if (xPos != std::string::npos) {
        isCapture = true;
        sanMove.erase(xPos, 1); // remove x
    }

    if (std::isupper(sanMove[0])) {
        movedPiece = sanMove[0]; // K Q R B N
        sanMove.erase(0, 1);
    }

    if (sanMove.length() >= 2) {
        char file = sanMove[sanMove.length() - 2]; // a-h
        char rank = sanMove[sanMove.length() - 1]; // 1-8

        endPos = Position<>(file - 'a', rank - '1');
        sanMove.erase(sanMove.length() - 2, 2);
    }

    char fileDisambiguity = '\0';
    char rankDisambiguity = '\0';

    if (sanMove.length() == 1) {
        if (std::isalpha(sanMove[0])) fileDisambiguity = sanMove[0];
        else if (std::isdigit(sanMove[0])) rankDisambiguity = sanMove[0];
    } else if (sanMove.length() == 2) {
        fileDisambiguity = sanMove[0];
        rankDisambiguity = sanMove[1]; 
    }

    startPos = board.findStartSquare(movedPiece, isWhiteToMove, endPos, fileDisambiguity, rankDisambiguity);

    if (isCapture) {
        capturedPiecePtr = board.getPiece(endPos);

        if (movedPiece == 'P' && capturedPiecePtr == nullptr) {
            isEnPassant = true;
            Position<> epPawnPos(endPos.x, startPos.y);
            capturedPiecePtr = board.getPiece(epPawnPos);
        }
    }

    return Move(startPos, endPos, isCapture, isCastle, isEnPassant, isCheck, movedPiece, promotedTo, capturedPiecePtr);
}   