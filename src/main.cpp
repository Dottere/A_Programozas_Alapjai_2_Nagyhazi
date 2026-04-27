#include "renderer.hpp"
#include "board.hpp"
#include "gamemaster.hpp"

int main(void) {   
    Board board;
    Renderer renderer(board);

    GameMaster gamemaster(board, renderer);

    // Kezdőállapot betöltése FEN alapján
    board.initialSetup();

    gamemaster.gameLoop();

    return 0;
}