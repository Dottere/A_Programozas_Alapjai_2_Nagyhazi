#pragma once

#include "chesstypes.hpp"

#include <filesystem>
#include <string>
#include <utility>
#include <vector>
#include <optional>

class Board;

namespace PGNHandler
{

    [[nodiscard]] std::optional<std::pair<PGNMetadata, std::vector<Move>>> parseFile(
        std::filesystem::path filePath,
        const Board &initialBoard);

    [[nodiscard]] std::string generatePGN(
        const PGNMetadata &metadata,
        const std::vector<Move> &history);

    [[nodiscard]] void savePGN(PGNMetadata &metadata, std::vector<Move>& moveHistory);
};

