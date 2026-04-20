#include "main.hpp"

#include <iostream>

#include "renderer.hpp"
#include "board.hpp"
#include "gamemaster.hpp"

int main(void) {   
    Board board;
    Renderer renderer(board);

    GameMaster gamemaster(board, renderer);

    // Kezdőállapot betöltése FEN alapján
    std::string startFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    
    if (board.loadFromFEN(startFEN)) {
        std::cout << "Kezdoallapot sikeresen betoltve!\n\n";
    } else {
        std::cerr << "Hiba a FEN betoltese soran!\n";
    }

    gamemaster.gameLoop();

    return 0;
}