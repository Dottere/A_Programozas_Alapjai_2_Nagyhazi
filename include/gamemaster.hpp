#ifndef GAMEMASTER_HPP
#define GAMEMASTER_HPP

#include "board.hpp"
#include "renderer.hpp"
#include "piece.hpp"
#include "chesstypes.hpp"

#include <vector>
#include <string>



class GameMaster {
    Board& board;
    Renderer& renderer;

    std::vector<Move> moveHistory;

    bool isValidInput(std::string userInput);

public:

    GameMaster(Board& board, Renderer& renderer) : board(board), renderer(renderer) {;}

    void run(const std::string& fenStr);
    void gameLoop(PGNMetadata& metadata);

    bool processMove(Position<> startPos, Position<> endPos, char promotedTo = '\0');

    inline std::vector<Move> getMoveHistory() const {
        return moveHistory;
    }

};

#endif