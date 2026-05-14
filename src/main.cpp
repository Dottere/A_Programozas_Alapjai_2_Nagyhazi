#include "renderer.hpp"
#include "board.hpp"
#include "gamemaster.hpp"
#include "pgnhandler.hpp"
#include "constants.hpp"
#include "utility.hpp"

#include <vector>
#include <string>
#include <string_view>
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <charconv>

namespace
{

    constexpr std::string_view HELP_STRING = R"(Usage: cli-chess [OPTIONS]

A CLI based chess game with support to loading FEN/PGN games

Options:
    -h, --help                          Prints this help
    -f, --fen <string>                  Loads a game from the provided FEN string
    -p, --pgn <file path>               Loads a game from the provided PGN allowing you to analyze is step by step)
    -t, --time-control <1|5|10|60>      Sets the time control of the game to <1|5|10|60> minutes)";

    bool parseArguments(int argc, char *argv[], std::string &fenStr, std::string &pgnFilePath, std::string &timeControl)
    {
        std::vector<std::string_view> args(argv, argv + argc);

        for (size_t i = 1; i < args.size(); i++)
        {
            if (is_any_of(args[i], "--help", "-h"))
            {
                std::cout << HELP_STRING << std::endl;
                return false;
            }
            else if (is_any_of(args[i], "--fen", "-f"))
            {
                if (++i != args.size())
                    fenStr = std::string(args[i]);
                else
                {
                    std::cerr << "Nincs megadva FEN sztring!" << std::endl;
                    return false;
                }
            }
            else if (is_any_of(args[i], "--pgn", "-p"))
            {
                if (++i != args.size())
                {
                    pgnFilePath = std::string(args[i]);
                }
                else
                {
                    std::cerr << "Nincs megadva PGN elérési út!";
                    return false;
                }
            }
            else if (args[i].compare(0, 2, "-t") == 0 || args[i] == "--time-control")
            {
                std::string_view value;

                if (args[i] == "--time-control" || args[i] == "-t")
                {
                    if (++i < args.size())
                    {
                        value = args[i];
                    }
                    else
                    {
                        std::cerr << "Nincs megadva időkorlát!" << std::endl;
                        return false;
                    }
                }
                else
                {
                    value = args[i].substr(2);
                }

                value = trim(value);

                if (is_any_of(value, "1", "5", "10", "60"))
                {
                    timeControl = std::string(value);
                }
                else
                {
                    std::cerr << "Helytelen időformátum: " << value << std::endl;
                    return false;
                }
            }
            else
            {
                std::cerr << "Ismeretlen paraméter: " << args[i] << std::endl;
                return false;
            }
        }
        return true;
    }
}

int main(int argc, char *argv[])
{
    std::string fenStr = std::string(DEFAULT_FEN);
    std::string pgnFilePath;
    std::string timeControl;

    if (!parseArguments(argc, argv, fenStr, pgnFilePath, timeControl))
    {
        return EXIT_FAILURE;
    }

    Board board;
    if (!board.loadFromFEN(fenStr))
    {
        std::cerr << "Helytelen FEN sztring!\n";
        return EXIT_FAILURE;
    }

    Renderer renderer(board);
    PGNHandler pgnhandler;
    GameMaster gamemaster(board, renderer, pgnhandler, timeControl);

    gamemaster.run(fenStr, pgnFilePath);

    return EXIT_SUCCESS;
}
