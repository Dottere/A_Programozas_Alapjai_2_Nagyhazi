#include <iostream>

#include "renderer.hpp"
#include "board.hpp"
#include "gamemaster.hpp"
#include "piece.hpp"

int main(void) {   
    Board board;
    Renderer renderer(board);

    GameMaster gamemaster(board, renderer);

    // --- Kezdőállapot betöltése FEN alapján ---
    std::string startFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    
    if (board.loadFromFEN(startFEN)) {
        std::cout << "Kezdoallapot sikeresen betoltve!\n\n";
    } else {
        std::cerr << "Hiba a FEN betoltese soran!\n";
    }

    // --- Bábu osztály tesztelése ---
    Position<> e2(6, 4);
    board.placePiece(std::make_unique<Pawn>(Color::WHITE), e2);

    gamemaster.processMove(Position<>(6, 4), Position<>(4, 4));

    gamemaster.gameLoop();

    return 0;
}