#include "pgnhandler.hpp"
#include "gamemaster.hpp"
#include "logger.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <optional>

namespace
{
    const std::regex META_REGEX("\\[([a-zA-Z]+)\\s+\"([^\"]*)\"\\]");
    const std::regex COMMENT_REGEX(R"(\{.*?\})");
    const std::regex VARIATION_REGEX(R"(\(.*?\))");
    std::regex MOVE_NUM_REGEX(R"(\b\d+\.+ *)");

    /**
     * @brief Tokenizálja a fennmaradó megtisztított SAN kifejezést (standard algebraic notation)
     *
     * Megkeresi a bábukat [KQRBN], oszlopokat [a-h], sorokat [1-8], elfogásokat (x), sakkokat (+), mattot (#),
     * és a sáncolást (O-O vagy O-O-O)
     */
    std::regex SAN_REGEX(R"([KQRBN]?[a-h]?[1-8]?x?[a-h][1-8](?:=[QRBN])?[\+#]?|O-O(?:-O)?)");

    std::optional<Move> sanToMoveObj(std::string sanMove, Board &board, bool isWhiteToMove)
    {

        if (sanMove.empty()) return std::nullopt;

        bool isCapture = false;
        bool isCastle = false;
        bool isEnPassant = false;
        bool isCheck = false;
        bool isCheckMate = false;

        char movedPiece = 'P';
        char promotedTo = '\0';

        Position<> startPos{-1, -1};
        Position<> endPos{-1, -1};
        Piece *capturedPiecePtr = nullptr;

        if (sanMove.back() == '#')
        {
            isCheckMate = true;
            sanMove.pop_back();
        }
        else if (sanMove.back() == '+')
        {
            isCheck = true;
            sanMove.pop_back();
        }

        if (sanMove == "O-O" || sanMove == "O-O-O")
        {
            isCastle = true;
            movedPiece = 'K';

            int rank = isWhiteToMove ? 0 : 7;
            startPos = Position<>(4, rank);

            if (sanMove == "O-O")
            {
                endPos = Position<>(6, rank); // kingside
            }
            else
            {
                endPos = Position<>(2, rank); // queenside
            }

            Move::Flags flags{
                .isCapture = isCapture,
                .isCastle = isCastle,
                .isEnPassant = isEnPassant,
                .isCheck = isCheck,
                .isCheckMate = isCheckMate,
            };

            return Move{startPos, endPos, flags, movedPiece, promotedTo, nullptr};
        }

        size_t equalPos = sanMove.find('=');
        if (equalPos != std::string::npos)
        {
            promotedTo = sanMove.back();
            sanMove = sanMove.substr(0, equalPos);
        }

        size_t xPos = sanMove.find('x');
        if (xPos != std::string::npos)
        {
            isCapture = true;
            sanMove.erase(xPos, 1); // remove x
        }

        if (std::isupper(sanMove[0]))
        {
            movedPiece = sanMove[0]; // K Q R B N
            sanMove.erase(0, 1);
        }

        if (sanMove.length() >= 2)
        {
            char file = sanMove[sanMove.length() - 2]; // a-h
            char rank = sanMove[sanMove.length() - 1]; // 1-8

            endPos = Position<>(file - 'a', rank - '1');
            sanMove.erase(sanMove.length() - 2, 2);
        }

        char fileDisambiguity = '\0';
        char rankDisambiguity = '\0';

        if (sanMove.length() == 1)
        {
            if (std::isalpha(sanMove[0]))
                fileDisambiguity = sanMove[0];
            else if (std::isdigit(sanMove[0]))
                rankDisambiguity = sanMove[0];
        }
        else if (sanMove.length() == 2)
        {
            fileDisambiguity = sanMove[0];
            rankDisambiguity = sanMove[1];
        }

        auto foundPos = board.findStartSquare(movedPiece, isWhiteToMove, endPos, fileDisambiguity, rankDisambiguity);
        if (foundPos)
        {
            startPos = *foundPos;
        }
        else
        {
            return std::nullopt;
        }

        if (isCapture)
        {
            capturedPiecePtr = board.getPiece(endPos);

            if (movedPiece == 'P' && capturedPiecePtr == nullptr)
            {
                isEnPassant = true;
                Position<> epPawnPos(endPos.x, startPos.y);
                capturedPiecePtr = board.getPiece(epPawnPos);
            }
        }

        Move::Flags flags{isCapture, isCastle, isEnPassant, isCheck, isCheckMate};
        
        return Move{startPos, endPos, flags, movedPiece, promotedTo, capturedPiecePtr};
    }

    char getDisambiguation(Board &board, const Move &m, bool isWhiteToMove)
    {
        if (m.movedPiece == 'P' || m.movedPiece == 'K')
            return '\0';

        char pieceTypeToFind = isWhiteToMove ? std::toupper(m.movedPiece) : std::tolower(m.movedPiece);
        bool collisionFound = false;
        bool shareFile = false;

        for (int x = 0; x < BOARD_SIZE; x++)
        {
            for (int y = 0; y < BOARD_SIZE; y++)
            {
                Position<> currentPos(x, y);

                if (currentPos == m.startPos)
                    continue;

                Piece *otherPiece = board.getPiece(currentPos);

                if (otherPiece && otherPiece->getPieceType() == pieceTypeToFind)
                {
                    if (otherPiece->isValidMove(currentPos, m.endPos, board.getPiece(m.endPos)))
                    {
                        if (otherPiece->canJump() || board.isPathClear(currentPos, m.endPos))
                        {
                            collisionFound = true;
                            if (currentPos.x == m.startPos.x)
                            {
                                shareFile = true;
                            }
                        }
                    }
                }
            }
        }

        if (!collisionFound)
            return '\0';

        if (shareFile)
            return '1' + m.startPos.y;

        return 'a' + m.startPos.x;
    }
}

std::pair<PGNMetadata, std::vector<Move>> PGNHandler::parseFile(std::filesystem::path filePath, const Board &initialBoard)
{
    PGNMetadata metadata;
    std::vector<Move> parsedMoves;
    std::ifstream file(filePath);
    std::string line;

    if (!file.is_open())
    {
        std::cerr << "Hiba: Nem lehetett megnyitni a(z) " << filePath << " elérési úton lévő fájlt!" << std::endl;
        return {metadata, parsedMoves};
    }

    std::ostringstream rawTextStream;

    std::smatch metaMatch;

    while (std::getline(file, line))
    {
        if (line.empty()) continue;

        if (line[0] == '[')
        {
            if (std::regex_search(line, metaMatch, META_REGEX))
            {
                std::string key = metaMatch[1].str();
                std::string value = metaMatch[2].str();

                if (key == "Event")        metadata.event = value;
                else if (key == "Site")    metadata.site = value;
                else if (key == "Date")    metadata.date = value;
                else if (key == "Round")   metadata.round = value;
                else if (key == "White")   metadata.white = value;
                else if (key == "Black")   metadata.black = value;
                else if (key == "Result")  metadata.result = value;
                else if (key == "FEN")     metadata.fen = value;
            }
            continue;
        }

        rawTextStream << line << " ";
    }
    file.close();

    std::string rawMoveText = rawTextStream.str();
    rawMoveText = std::regex_replace(rawMoveText, COMMENT_REGEX, "");   // remove comments
    rawMoveText = std::regex_replace(rawMoveText, VARIATION_REGEX, ""); // remove variations
    rawMoveText = std::regex_replace(rawMoveText, MOVE_NUM_REGEX, "");  // remove move numbers

    auto words_begin = std::sregex_iterator(rawMoveText.begin(), rawMoveText.end(), SAN_REGEX);
    auto words_end = std::sregex_iterator();

    bool isWhiteToMove = true;

    Board simBoard = initialBoard;

    for (std::sregex_iterator i = words_begin; i != words_end; i++)
    {
        std::string sanMove = i->str();

        LOG_DEBUG("sanMove: " + sanMove);
        
        auto currentMove = sanToMoveObj(sanMove, simBoard, isWhiteToMove).value_or(Move());

        if (currentMove.startPos.x == -1)
        {
            std::cerr << "Hiba: Hibás PGN lépés: " << sanMove << ". Lehet, hogy hibás a PGN?\n";
            break;
        }

        // 2. make the move on the board
        simBoard.movePiece(currentMove.startPos, currentMove.endPos);

        if (currentMove.flags.isCastle)
        {
            if (currentMove.endPos.x == 6) // kingside
            {
                simBoard.movePiece(Position<>(7, currentMove.startPos.y), Position<>(5, currentMove.startPos.y));
            }
            else // queenside
            {
                simBoard.movePiece(Position<>(0, currentMove.startPos.y), Position<>(3, currentMove.startPos.y));
            }
        }

        // 3. save to parsed moves vector
        parsedMoves.push_back(currentMove);

        isWhiteToMove = !isWhiteToMove;
    }

    return {metadata, parsedMoves};
}

std::string PGNHandler::generatePGN(const PGNMetadata &metadata, const std::vector<Move> &history)
{
    std::ostringstream oss;

    oss << "[Event \"" << metadata.event << "\"]\n";
    oss << "[Site \"" << metadata.site << "\"]\n";
    oss << "[Date \"" << metadata.date << "\"]\n";
    oss << "[Round \"" << metadata.round << "\"]\n";
    oss << "[White \"" << metadata.white << "\"]\n";
    oss << "[Black \"" << metadata.black << "\"]\n";
    oss << "[Result \"" << metadata.result << "\"]\n";

    if (!metadata.fen.empty())
    {
        oss << "[SetUp \"1\"]\n";
        oss << "[FEN \"" << metadata.fen << "\"]\n";
    }
    oss << "\n";

    Board simBoard;
    simBoard.loadFromFEN(std::string(DEFAULT_FEN));

    int halfMoveCount = 0;

    for (const auto &m : history)
    {
        bool isWhiteToMove = (halfMoveCount % 2 == 0);

        if (isWhiteToMove)
        {
            oss << std::to_string((halfMoveCount / 2) + 1) << ". ";
        }

        if (m.flags.isCastle)
        {
            oss << ((m.endPos.x == 6) ? "O-O" : "O-O-O");
        }
        else
        {
            if (m.movedPiece != 'P')
            {
                oss << m.movedPiece;
                char disambig = getDisambiguation(simBoard, m, isWhiteToMove);
                if (disambig != '\0') oss << disambig;
            }
            else if (m.flags.isCapture || m.flags.isEnPassant)
            {
                oss << (char)('a' + m.startPos.x);
            }

            if (m.flags.isCapture || m.flags.isEnPassant) oss << "x";

            oss << (char)('a' + m.endPos.x) << (char)('1' + m.endPos.y);

            if (m.promotedTo != '\0' && m.promotedTo != ' ')
            {
                oss << "=" << m.promotedTo;
            }
        }

        if (m.flags.isCheckMate) oss << "#";
        else if (m.flags.isCheck) oss << "+";

        oss << " ";

        if (m.flags.isEnPassant) simBoard.removePiece(Position<>(m.endPos.x, m.startPos.y));

        simBoard.movePiece(m.startPos, m.endPos);

        if (m.flags.isCastle)
        {
            if (m.endPos.x == 6) simBoard.movePiece(Position<>(7, m.startPos.y), Position<>(5, m.startPos.y));
            else simBoard.movePiece(Position<>(0, m.startPos.y), Position<>(3, m.startPos.y));
        }

        halfMoveCount++;
    }

    std::string resultStr = oss.str();
    if (resultStr.empty() && resultStr.back() == ' ') resultStr.pop_back();

    resultStr += " " + metadata.result + "\n";

    return resultStr;
}
