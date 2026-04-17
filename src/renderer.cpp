#include <iostream>
#include "renderer.hpp"

void Renderer::display() {
    for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
        const Piece* p = board.getPiece(x, y);
        if (p) std::cout << "P "; // Ide jöhetnek az Unicode karakterek
        else std::cout << ". ";
    }
    std::cout << std::endl;
}
}