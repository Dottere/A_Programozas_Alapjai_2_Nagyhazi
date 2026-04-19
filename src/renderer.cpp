#include "renderer.hpp"

#include <iostream>

void Renderer::display() {
    for (int y = 7; y >= 0; y--) { 
        for (int x = 0; x < 8; x++) {
            const Piece* p = board.getPiece(Position(x, y));
            
            if (p) {
                std::cout << p->getSymbol() << " "; 
            } else {
                std::cout << "\uFF0E"; // Fullwidth dot (U+FF0E)
            }
        }
        std::cout << std::endl;
    }
}