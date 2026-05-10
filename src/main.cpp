#include "main.hpp"

#include "renderer.hpp"
#include "board.hpp"
#include "gamemaster.hpp"
#include "pgnhandler.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cctype>

int main(int argc, char* argv[]) {   
    std::vector<std::string> args(argv, argv + argc);

    std::string fenStr = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    for (size_t i = 1; i < args.size(); i++) {
        if (args[i] == "--help" || args[i] == "-h") {
            std::cout << helpString << std::endl;
            return 0;
        }
        else if (args[i] == "--fen" || args[i] == "-f") {
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
    PGNHandler pgnhandler;


    gamemaster.gameLoop();

    char answer = ' ';
    std::cout << "Would you like to save the game PGN to file? (y/n)" << std::endl;
    while (std::tolower(answer) != 'y' && std::tolower(answer) != 'n') {
        std::cin >> answer;
    }

    if (std::tolower(answer) == 'y') {
        std::cout << "Name of file you want to save to: ";
        std::string filename;
        std::cin >> filename;

        std::ofstream outFile(filename + ".pgn");
        if (outFile.is_open()) {
            outFile << pgnhandler.generatePGN(gamemaster.getMoveHistory());
            outFile.close();
            std::cout << "Game saved successfully to " << filename << ".pgn!" << std::endl;
        } else {
            std::cerr << "Error: Could not open file for writing" << std::endl;
        }
    }

    return 0;
}