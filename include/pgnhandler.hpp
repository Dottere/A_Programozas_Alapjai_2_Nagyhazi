#ifndef PGNHANDLER_HPP
#define PGNHANDLER_HPP

#include "gamemaster.hpp"
#include "board.hpp"

#include <string>
#include <vector>

class PGNHandler {

    Move sanToMoveObj(std::string sanMove, Board& board, bool isWhiteToMove);

    char getDisambiguation(Board& board, const Move& m, bool isWhiteToMove);

    public:
        std::vector<Move> parseFile(std::string filePath, Board& board);

        std::string generatePGN(const std::vector<Move>& history);
};

#endif