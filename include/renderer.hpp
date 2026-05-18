#pragma once

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
private:
    const Board &board;

    void renderInternal(bool showUI, int whitePoints = 0, int blackPoints = 0) const;


public:
    Renderer(const Board &board) : board(board) {}
    
    void display(int whitePoints = 0, int blackPoints = 0) const;
    void display() const;
};