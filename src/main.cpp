#include "renderer.hpp"
#include "board.hpp"
#include "gamemaster.hpp"

#include <iostream>
#include <vector>
#include <string>

int main(int argc, char* argv[]) {   
    std::vector<std::string> args(argv, argv + argc);

    std::string fenStr = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    for (size_t i = 1; i < args.size(); i++) {
        if (args[i] == "--fen" || args[i] == "-f") {
            fenStr = args[++i];
        }
        else {
            std::cout << "Unknown argument: 1" << args[i] << std::endl;
        }
    }


    Board board;

    // Kezdőállapot betöltése FEN alapján
    board.initialSetup(fenStr);

    Renderer renderer(board);
    GameMaster gamemaster(board, renderer);


    gamemaster.gameLoop();

    return 0;
}