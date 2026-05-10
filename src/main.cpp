#include "renderer.hpp"
#include "board.hpp"
#include "gamemaster.hpp"
#include "pgnhandler.hpp"
#include "chesstypes.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>

namespace {
std::string helpString = R"(Usage: cli-chess [OPTIONS]

A CLI based chess game with support to loading FEN/PGN games

Options:
    -h, --help                      Prints this help
    -f, --fen <string>              Loads a game from the provided FEN string
    -p, --pgn <file path>           Loads a game from the provided PGN allowing you to analyze is step by step)";

std::string defaultPosition = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

bool parseArguments(int argc, char* argv[], std::string& fenStr);
}

int main(int argc, char* argv[]) {   
    std::string fenStr = defaultPosition;

    if (!parseArguments(argc, argv, fenStr)) {
        return EXIT_FAILURE;
    }

    Board board;
    if (!board.loadFromFEN(fenStr)) {
        std::cerr << "Helytelen FEN sztring!\n";
        return EXIT_FAILURE;
    }

    Renderer renderer(board);
    GameMaster gamemaster(board, renderer);

    gamemaster.run(fenStr);
    
    return EXIT_SUCCESS;
}

namespace {
    bool parseArguments(int argc, char* argv[], std::string& fenStr) {
        std::vector<std::string> args(argv, argv + argc);

        for (size_t i = 1; i < args.size(); i++) {
            if (args[i] == "--help" || args[i] == "-h") {
                std::cout << helpString << std::endl;
                return false;
            }
            else if (args[i] == "--fen" || args[i] == "-f") {
                if (++i != args.size())
                    fenStr = args[i];
                else {
                    std::cerr << "Nincs megadva FEN sztring!" << std::endl;
                    return false;
                }
            }
            else {
                std::cerr << "Ismeretlen paraméter: " << args[i] << std::endl;
                return false;
            }
        }
        return true;
    }
}
    