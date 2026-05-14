#ifndef GAMEMASTER_HPP
#define GAMEMASTER_HPP

#include "board.hpp"
#include "renderer.hpp"
#include "piece.hpp"
#include "chesstypes.hpp"

#include <vector>
#include <string>
#include <chrono>

class GameMaster
{
    Board &board;
    Renderer &renderer;

    double whiteTimeRemaining; // seconds;
    double blackTimeRemaining;
    std::chrono::time_point<std::chrono::steady_clock> turnStartTime;

    int pointsWhite = 0;
    int pointsBlack = 0;

    std::vector<Move> moveHistory;

    bool isValidInput(std::string userInput);
    void gameLoop(PGNMetadata &metadata);
    bool processMove(Position<> startPos, Position<> endPos, char promotedTo = '\0');
    void replayPGN(const std::string &pgnFilePath);
    void manualPlay(const std::string &fenStr);

public:
    GameMaster(Board &board, Renderer &renderer, const std::string &timeControl)
        : board(board),
          renderer(renderer)
    {
        double seconds = 999999;

        if (timeControl.size() == 1)
        {
            seconds = static_cast<double>((timeControl[0] - '0') * 60);
        }
        else if (timeControl.size() == 2)
        {
            seconds = static_cast<double>((((timeControl[0] - '0') * 10) + (timeControl[1] - '0')) * 60);
        }

        whiteTimeRemaining = seconds;
        blackTimeRemaining = seconds;
    }

    void run(const std::string &fenStr, const std::string &pgnFilePath);
};

#endif