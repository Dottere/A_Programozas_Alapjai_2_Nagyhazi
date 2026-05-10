#include "renderer.hpp"
#include "board.hpp"
#include "gamemaster.hpp"
#include "pgnhandler.hpp"
#include "constants.hpp"

#include <vector>
#include <string>
#include <string_view>
#include <iostream>
#include <cstdlib>

namespace {

constexpr std::string_view HELP_STRING = R"(Usage: cli-chess [OPTIONS]

A CLI based chess game with support to loading FEN/PGN games

Options:
    -h, --help                      Prints this help
    -f, --fen <string>              Loads a game from the provided FEN string
    -p, --pgn <file path>           Loads a game from the provided PGN allowing you to analyze is step by step)";

bool parseArguments(int argc, char* argv[], std::string& fenStr, std::string& pgnFilePath) {
    std::vector<std::string_view> args(argv, argv + argc);

    for (size_t i = 1; i < args.size(); i++) {
        if (args[i] == "--help" || args[i] == "-h") {
            std::cout << HELP_STRING << std::endl;
            return false;
        }
        else if (args[i] == "--fen" || args[i] == "-f") {
            if (++i != args.size())
                fenStr = std::string(args[i]);
            else {
                std::cerr << "Nincs megadva FEN sztring!" << std::endl;
                return false;
            }
        }
        else if (args[i] == "--pgn" || args[i] == "-p") {
            if (++i != args.size()) {
                pgnFilePath = std::string(args[i]);
            } else {
                std::cerr << "Nincs megadva PGN elérési út!";
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

int main(int argc, char* argv[]) {   
    std::string fenStr = std::string(DEFAULT_FEN);
    std::string pgnFilePath;

    if (!parseArguments(argc, argv, fenStr, pgnFilePath)) {
        return EXIT_FAILURE;
    }

    Board board;
    if (!board.loadFromFEN(fenStr)) {
        std::cerr << "Helytelen FEN sztring!\n";
        return EXIT_FAILURE;
    }

    Renderer renderer(board);
    PGNHandler pgnhandler;
    GameMaster gamemaster(board, renderer, pgnhandler);

    gamemaster.run(fenStr, pgnFilePath);
    
    return EXIT_SUCCESS;
}
    