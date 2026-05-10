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
#include <cstdlib>

int main(int argc, char* argv[]) {   
    std::vector<std::string> args(argv, argv + argc);

    std::string fenStr = defaultPosition;

    for (size_t i = 1; i < args.size(); i++) {
        if (args[i] == "--help" || args[i] == "-h") {
            std::cout << helpString << std::endl;
            return EXIT_SUCCESS;
        }
        else if (args[i] == "--fen" || args[i] == "-f") {
            if (++i != args.size())
                fenStr = args[i];
            else {
                std::cerr << "Nincs megadva FEN sztring!" << std::endl;
                return EXIT_FAILURE;
            }
        }
        else {
            std::cerr << "Ismeretlen paraméter: " << args[i] << std::endl;
            return EXIT_FAILURE;
        }
    }


    Board board;

    // Kezdőállapot betöltése FEN alapján
    if (!board.loadFromFEN(fenStr)) {
        std::cerr << "Helytelen FEN sztring!";
        return EXIT_FAILURE;
    };

    Renderer renderer(board);
    GameMaster gamemaster(board, renderer);
    PGNHandler pgnhandler;


    gamemaster.gameLoop();

    char answer = ' ';
    std::cout << "Szeretnéd kimenteni a játékot PGN fájlba? (i/n)" << std::endl;
    while (std::tolower(answer) != 'i' && std::tolower(answer) != 'n') {
        std::cin >> answer;
    }

    if (std::tolower(answer) == 'i') {
        std::cout << "A fájl neve (kiterjesztés nélkül): ";
        std::string filename;
        std::cin >> filename;

        std::ofstream outFile(filename + ".pgn");
        if (outFile.is_open()) {
            outFile << pgnhandler.generatePGN(gamemaster.getMoveHistory());
            outFile.close();
            std::cout << "A játék sikeresen elmentve a " << filename << ".pgn fájlba!" << std::endl;
        } else {
            std::cerr << "Hiba: Nem lehetett megnyitni a fájlt az íráshoz!" << std::endl;
        }
    }

    return EXIT_SUCCESS;
}