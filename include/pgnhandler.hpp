#ifndef PGNHANDLER_HPP
#define PGNHANDLER_HPP

#include "chesstypes.hpp"

#include <filesystem>
#include <string>
#include <utility>
#include <vector>

class Board;

class PGNHandler
{
public:
    [[nodiscard]] static std::pair<PGNMetadata, std::vector<Move>> parseFile(
        std::filesystem::path filePath,
        const Board &initialBoard
    );

    [[nodiscard]] static std::string generatePGN(
        const PGNMetadata &metadata,
        const std::vector<Move> &history
    );
};

#endif