#include "renderer.hpp"

#include "piece.hpp"

#include <iostream>

void Renderer::display() {
    // colour setup
    const std::string RESET = "\033[0m";
    const std::string P1_COLOR = "\033[97m"; // Bright White Text
    const std::string P2_COLOR = "\033[30m"; // Black Text
    const std::string BG_WHITE = "\033[47m"; // Light Grey Background (Light Squares)
    const std::string BG_DARK   = "\033[42m"; // Green Background (Dark Squares)

    // header
    std::cout << "  ";
    for (int x = 0; x < 8; x++) {
        std::cout << (char)('A'+x) << " ";
    }
    std::cout << std::endl;

    for (int y = 7; y >= 0; y--) { 
        for (int x = 0; x < 8; x++) {
            const Piece* p = board.getPiece(Position(x, y));

            // left border
            if (x == 0) std::cout << y+1 << " ";

            if ((x + y) % 2 != 0) { 
                std::cout << BG_WHITE; 
            } else {
                std::cout << BG_DARK; 
            }

            if (p) {
                if (p->getColor() == Color::WHITE)
                    std::cout << P1_COLOR << p->getSymbol() << " " << RESET;
                else
                    std::cout << P2_COLOR << p->getSymbol() << " " << RESET; 
            } else {
                std::cout << "  " << RESET; 
            }

            // "UI" elements
            if (x == 7) {
                // right border
                std::cout << " " << y+1;

                std::string padding = "     ";

                if (y == 7) std::cout << padding << "Turn: " << board.getTurn();
                else if (y == 5) {
                    std::cout << "     Captured White: ";
                    for (const auto& p : board.getWhiteCaptured()) {
                        if (p) std::cout << P1_COLOR << p->getSymbol() << " " << RESET;
                    }
                }
                else if (y == 3) {
                    std::cout << "     Captured Black: ";
                    for (const auto& p : board.getBlackCaptured()) {
                        if (p) std::cout << P2_COLOR << p->getSymbol() << " " << RESET;
                    }
                }
                
            }

        }
        std::cout << std::endl;
    }

        // footer
        std::cout << "  ";
        for (int x = 0; x < 8; x++) {
            std::cout << (char)('A'+x) << " ";
        }

        std::cout << std::endl;
}
