#pragma once

#include "board.hpp"
#include "renderer.hpp"
#include "piece.hpp"
#include "chesstypes.hpp"

#include <vector>
#include <string_view>

class GameMaster
{
private:
    Board &board;
    const Renderer &renderer;

    int pointsWhite = 0;
    int pointsBlack = 0;

    std::vector<Move> moveHistory;

    void gameLoop(PGNMetadata &metadata);
    void manualPlay(std::string_view fenStr);
    void executeBoardMovement(const Move &move, int rookStartX, int rookEndX);
    void rollbackInvalidMove(const Move &move);
    void finalizeMove(Move &currentMove, int pointsGained = 0);


    [[nodiscard]] bool replayPGN(std::string_view pgnFilePath);
    [[nodiscard]] bool isValidInput(std::string_view userInput) const;
    void processMove(Position<> startPos, Position<> endPos, char promotedTo = '\0');
    [[nodiscard]] bool isValidActivePiece(const Piece* p) const;
    [[nodiscard]] bool isEnPassantMove(const Piece* p, Position<> startPos, Position<> endPos) const;
    [[nodiscard]] bool validateCastlingRules(Position<> startPos, Position<> endPos, int &rookStartX, int &rookEndX) const;

    [[nodiscard]] Move createMoveRecord
    (
        const Piece *p, 
        Position<> startPos, 
        Position<> endPos, 
        char promotedTo, 
        bool isEnPassant, 
        bool isCastle, 
        Piece *capturedPiece
    ) const;

public:
    GameMaster(Board &board, const Renderer &renderer)
        : board(board),
          renderer(renderer) {}

    void run(std::string_view fenStr, std::string_view pgnFilePath);
};