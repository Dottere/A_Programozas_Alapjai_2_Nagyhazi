#ifndef GAMEMASTER_HPP
#define GAMEMASTER_HPP

#include "board.hpp"
#include "renderer.hpp"
#include "piece.hpp"
#include "chesstypes.hpp"

#include <vector>
#include <string>

class PGNHandler;

class GameMaster {
    Board& board;
    Renderer& renderer;
    PGNHandler& pgnhandler;

    std::vector<Move> moveHistory;

    bool isValidInput(std::string userInput);
    void gameLoop(PGNMetadata& metadata);
    bool processMove(Position<> startPos, Position<> endPos, char promotedTo = '\0');
    void replayPGN(const std::string& pgnFilePath);
    void manualPlay(const std::string& fenStr);

public:

    GameMaster(Board& board, Renderer& renderer, PGNHandler& pgnhandler) : board(board), renderer(renderer), pgnhandler(pgnhandler) {}

    void run(const std::string& fenStr, const std::string& pgnFilePath);

};

#endif