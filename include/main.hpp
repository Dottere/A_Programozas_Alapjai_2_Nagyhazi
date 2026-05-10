#ifndef MAIN_HPP
#define MAIN_HPP

#include <iostream>

std::string helpString = R"(Usage: cli-chess [OPTIONS]

A CLI based chess game with support to loading FEN/PGN games

Options:
    -h, --help                      Prints this help
    -f, --fen <string>              Loads a game from the provided FEN string
    -p, --pgn <file path>           Loads a game from the provided PGN allowing you to analyze is step by step)";

std::string defaultPosition = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

#endif