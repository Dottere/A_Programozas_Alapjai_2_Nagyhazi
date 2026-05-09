#ifndef MAIN_HPP
#define MAIN_HPP

#include <iostream>

std::string helpString = R"(Usage: cli-chess [OPTIONS]

A CLI based chess game with support to loading FEN/PGN games
Options:
    -h, --help                      Prints this help
    -f, --fen <string>              Loads a game from the provided FEN string)";


#endif