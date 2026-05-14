#ifndef PGNHANDLER_HPP
#define PGNHANDLER_HPP

#include "gamemaster.hpp"
#include "board.hpp"
#include "chesstypes.hpp"

#include <string>
#include <vector>
#include <optional>

class PGNHandler
{
public:
    [[nodiscard]] std::pair<PGNMetadata, std::vector<Move>> parseFile(std::string filePath, Board &board);

    [[nodiscard]] std::string generatePGN(const PGNMetadata &metadata, const std::vector<Move> &history);
};

#endif