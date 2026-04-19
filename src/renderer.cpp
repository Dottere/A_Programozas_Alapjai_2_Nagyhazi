#include "renderer.hpp"

#include <iostream>

void Renderer::display() {
    const std::string RESET = "\033[0m";
    const std::string P1_COLOR = "\033[97m"; // Bright White
    const std::string P2_COLOR = "\033[31m"; // Red


    std::cout << "  ";
    for (int x = 0; x < 8; x++) {
        std::cout << (char)('A'+x) << " ";
    }
    std::cout << std::endl;

    for (int y = 7; y >= 0; y--) { 
        for (int x = 0; x < 8; x++) {
            const Piece* p = board.getPiece(Position(x, y));
            
            if (x == 0) std::cout << y+1 << " ";
            if (p) {
                if (p->getColor() == Color::WHITE)
                    std::cout << P1_COLOR << p->getSymbol() << RESET << " ";
                else
                    std::cout << P2_COLOR << p->getSymbol() << RESET << " "; 
            } else {
                std::cout << "  "; // Fullwidth dot (U+FF0E)
            }

            if (x == 7) std::cout << " " << y+1;
        }
        std::cout << std::endl;
    }
        std::cout << "  ";
        for (int x = 0; x < 8; x++) {
            std::cout << (char)('A'+x) << " ";
        }

        std::cout << std::endl;
}