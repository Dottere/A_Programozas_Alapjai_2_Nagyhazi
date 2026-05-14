#ifndef RENDERER_HPP
#define RENDERER_HPP

/*
 * Ez az osztály lesz felelős a grafikus megvalósításért, azért van elválasztva a többitől, hogy ha később
 * változtatni szeretnénk a kiírás menetén, akkor csak ezt kelljen leváltani.
 */

#include "board.hpp"

#include <string>
#include <sstream>
#include <iomanip>

class Renderer
{
    const Board &board;

public:
    Renderer(const Board &board) : board(board) {}
    void display(double whiteTime, double blackTime, int whitePoints, int blackPoints);
    void display();

    inline std::string formatTime(double totalSeconds)
    {
        if (totalSeconds < 0)
            totalSeconds = 0;
        int minutes = static_cast<int>(totalSeconds) / 60;
        int seconds = static_cast<int>(totalSeconds) % 60;

        std::stringstream ss;
        ss << std::setfill('0') << std::setw(2) << minutes << ":"
           << std::setfill('0') << std::setw(2) << seconds;

        return ss.str();
    }
};

#endif