#ifndef RENDERER_HPP
#define RENDERER_HPP

/*
* Ez az osztály lesz felelős a grafikus megvalósításért, azért van elválasztva a többitől, hogy ha később
* változtatni szeretnénk a kiírás menetén, akkor csak ezt kelljen leváltani.
*/

#include "board.hpp"
#include <iostream>

class Renderer {
    public:
        inline void display(const Board& board) {
            for (int x = 0; x < 8; x++) {
                for (int y = 0; y <= 8; y++) {
                    if (x==7 && y == 8) break;
                    if (y == 7) {
                        std::cout << "#\n" << std::endl; 
                        break;
                    }
                    std::cout << "#  ";
                }
            }
        }
};

#endif