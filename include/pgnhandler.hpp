#ifndef PGNHANDLER_HPP
#define PGNHANDLER_HPP

#include "gamemaster.hpp"
#include "board.hpp"

class PGNHandler {

    Move sanToMoveObj(std::string sanMove);
    
    public:
        std::vector<Move> parseFile(std::string filePath, Board& board);

        std::string generatePGN(const std::vector<Move>& history);
};

#endif