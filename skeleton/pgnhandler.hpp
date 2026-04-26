#ifndef PGNHANDLER_HPP
#define PGNHANDLER_HPP

#include "gamemaster.hpp"
#include "board.hpp"

#include <string>
#include <vector>

class PGNHandler {

    public:
        // Beolvas egy fájlból, és Move struktúrákat csinál a PGN-ből

        std::vector<Move> parseFile(std::string filePath, Board& board);

        // PGN-t csinál a játékban történt lépésekből (Move struktúrákból)
        
        std::string generatePGN(const std::vector<Move>& history);
};

#endif