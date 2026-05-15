#pragma once

#include "board.hpp"
#include "renderer.hpp"
#include "piece.hpp"
#include "chesstypes.hpp"

#include <vector>
#include <string_view>

class GameMaster
{
    Board &board;
    const Renderer &renderer;

    int pointsWhite = 0;
    int pointsBlack = 0;

    std::vector<Move> moveHistory;

    void gameLoop(PGNMetadata &metadata);
    void manualPlay(std::string_view fenStr);
    void replayPGN(std::string_view pgnFilePath);

    [[nodiscard]] bool isValidInput(std::string_view userInput) const;
    [[nodiscard]] bool processMove(Position<> startPos, Position<> endPos, char promotedTo = '\0');


public:
    GameMaster(Board &board, const Renderer &renderer)
        : board(board),
          renderer(renderer) {}

    void run(std::string_view fenStr, std::string_view pgnFilePath);
};