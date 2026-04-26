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
        Renderer(const Board& board);

        // Metódus ami kiírja a CLI-re a játékos számára releváns dolgokat (tábla, jelölések)
        
        void display();
};

#endif