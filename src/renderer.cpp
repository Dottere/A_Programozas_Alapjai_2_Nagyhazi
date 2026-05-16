#include "renderer.hpp"
#include "piece.hpp"
#include <iostream>

// The public functions simply delegate to the internal renderer
void Renderer::display(int whitePoints, int blackPoints) const
{
    renderInternal(true, whitePoints, blackPoints);
}

void Renderer::display() const
{
    renderInternal(false);
}

// The unified internal renderer
void Renderer::renderInternal(bool showUI, int whitePoints, int blackPoints) const
{
    // clear the screen and reset cursor
    std::cout << "\033[2J\033[H" << '\n';

    bool isWhitePOV = (board.getTurn() == Color::WHITE);

    std::cout << "  ";
    for (int col = 0; col < 8; col++)
    {
        int x = isWhitePOV ? col : 7 - col; // Flip files for Black
        std::cout << static_cast<char>('A' + x) << " ";
    }
    std::cout << '\n';

    for (int row = 7; row >= 0; row--)
    {
        int y = isWhitePOV ? row : 7 - row;

        // left border
        std::cout << y + 1 << " ";

        for (int col = 0; col < 8; col++)
        {
            int x = isWhitePOV ? col : 7 - col; // Flip files for Black

            const Piece *p = board.getPiece(Position<>(x, y));

            if ((x + y) % 2 != 0)
            {
                std::cout << BG_WHITE;
            }
            else
            {
                std::cout << BG_DARK;
            }

            if (p)
            {
                if (p->getColor() == Color::WHITE)
                    std::cout << P1_COLOR << p->getSymbol() << " " << RESET;
                else
                    std::cout << P2_COLOR << p->getSymbol() << " " << RESET;
            }
            else
            {
                std::cout << "  " << RESET;
            }
        }

        // right border
        std::cout << " " << y + 1;

        if (showUI)
        {
            std::string padding = "     ";

            if (row == 7)
            {
                std::cout << padding << "Következik: " << board.getTurn();
            }
            else if (row == 5)
            {
                std::cout << padding << "Fehér által leütött: ";
                for (const auto &p : board.getBlackCaptured())
                {
                    if (p)
                        std::cout << P2_COLOR << p->getSymbol() << " " << RESET;
                }
                if (whitePoints > blackPoints)
                {
                    std::cout << P1_COLOR << " (+" << (whitePoints - blackPoints) << ")" << RESET;
                }
            }
            else if (row == 3)
            {
                std::cout << padding << "Sötét által leütött: ";
                for (const auto &p : board.getWhiteCaptured())
                {
                    if (p)
                        std::cout << P1_COLOR << p->getSymbol() << " " << RESET;
                }
                if (blackPoints > whitePoints)
                {
                    std::cout << P1_COLOR << " (+" << (blackPoints - whitePoints) << ")" << RESET;
                }
            }
        }

        std::cout << '\n';
    }

    // footer
    std::cout << "  ";
    for (int col = 0; col < 8; col++)
    {
        int x = isWhitePOV ? col : 7 - col; // Flip files for Black
        std::cout << static_cast<char>('A' + x) << " ";
    }

    // footer UI logic
    if (showUI)
    {
        std::cout << "       " << "Összpontszám: " << whitePoints << " | " << blackPoints;
    }

    std::cout << std::endl;
}