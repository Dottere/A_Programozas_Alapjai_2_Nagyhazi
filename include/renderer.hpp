#pragma once

/*
 * Ez az osztály lesz felelős a grafikus megvalósításért, azért van elválasztva a többitől, hogy ha később
 * változtatni szeretnénk a kiírás menetén, akkor csak ezt kelljen leváltani.
 */

#include "board.hpp"

#include <string>

class Renderer
{
    const Board &board;

public:
    Renderer(const Board &board) : board(board) {}
    void display(int whitePoints, int blackPoints);
    void display();
};