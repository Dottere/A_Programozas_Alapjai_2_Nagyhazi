#ifndef PGNHANDLER_HPP
#define PGNHANDLER_HPP

#include "gamemaster.hpp"
#include "board.hpp"
#include "chesstypes.hpp"

#include <string>
#include <vector>
#include <optional>

class PGNHandler {

    std::optional<Move> sanToMoveObj(std::string sanMove, Board& board, bool isWhiteToMove);

    char getDisambiguation(Board& board, const Move& m, bool isWhiteToMove);

    public:
        std::pair<PGNMetadata, std::vector<Move>> parseFile(std::string filePath, Board& board);

        std::string generatePGN(const PGNMetadata& metadata, const std::vector<Move>& history);
};

#endif