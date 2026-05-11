#include "renderer.hpp"

#include "piece.hpp"

#include <iostream>

void Renderer::display(double whiteTime, double blackTime) {
    // colour setup
    const std::string RESET = "\033[0m";
    const std::string P1_COLOR = "\033[97m"; // Bright White Text
    const std::string P2_COLOR = "\033[30m"; // Black Text
    const std::string BG_WHITE = "\033[47m"; // Light Grey Background (Light Squares)
    const std::string BG_DARK   = "\033[42m"; // Green Background (Dark Squares)

    bool isWhitePOV = (board.getTurn() == Color::WHITE);

    // header
    std::cout << "  ";
    for (int col = 0; col < 8; col++) {
        int x = isWhitePOV ? col : 7 - col; // Flip files for Black
        std::cout << (char)('A'+x) << " ";
    }
    std::cout << std::endl;

    for (int row = 7; row >= 0; row--) { 
        int y = isWhitePOV ? row : 7 - row; 

        // left border
        std::cout << y + 1 << " ";

        for (int col = 0; col < 8; col++) {
            int x = isWhitePOV ? col : 7 - col; // Flip files for Black

            const Piece* p = board.getPiece(Position<>(x, y));

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
        }

        std::cout << " " << y + 1;

        // UI
        std::string padding = "     ";

        if (row == 7) std::cout << padding << "Következik: " << board.getTurn();
        else if (row == 5) {
            std::cout << padding << "Fehér által leütött: ";
            for (const auto& p : board.getWhiteCaptured()) {
                if (p) std::cout << P1_COLOR << p->getSymbol() << " " << RESET;
            }
        }
        else if (row == 3) {
            std::cout << padding << "Sötét által leütött: ";
            for (const auto& p : board.getBlackCaptured()) {
                if (p) std::cout << P2_COLOR << p->getSymbol() << " " << RESET;
            }
        }
        else if (row == 1) {
            if (whiteTime <= 999.0) {
                std::cout << padding << "[" << formatTime(whiteTime) << " : " << formatTime(blackTime) << "]";
            }
        }

        std::cout << std::endl;
    }
    
    // footer
    std::cout << "  ";
    for (int col = 0; col < 8; col++) {
        int x = isWhitePOV ? col : 7 - col; // Flip files for Black
        std::cout << (char)('A'+x) << " ";
    }

    std::cout << std::endl;
} 
