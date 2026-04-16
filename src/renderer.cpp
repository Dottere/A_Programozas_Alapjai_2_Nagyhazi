#include <iostream>
#include "renderer.hpp"

void Renderer::display() {
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y <= 8; y++) {
            if (x==7 && y == 8) break;
            if (y == 7) {
                std::cout << "\u2654\n" << std::endl; 
                break;
            }
            std::cout << "\u2654   ";
        }
    }
}