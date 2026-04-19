#include "pgnhandler.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>



std::vector<Move> PGNHandler::parseFile(std::string filePath, Board& board) {
    std::vector<Move> parsedMoves;
    std::ifstream file(filePath);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Hiba: Nem lehetett megnyitni a(z) " << filePath << " elérési úton lévő fájlt!" << std::endl;
        return parsedMoves;
    }

    std::string rawMoveText = "";

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        if (line[0] == '[') continue; // skip pgn metadata

        rawMoveText += line + " ";
    }
    file.close();

    rawMoveText = std::regex_replace(rawMoveText, std::regex(R"(\{.*?\})"), ""); // remove comments
    rawMoveText = std::regex_replace(rawMoveText, std::regex(R"(\(.*?\))"), ""); // remove variations
    rawMoveText = std::regex_replace(rawMoveText, std::regex(R"(\b\d+\.+ *)"), ""); // remove move numbers

    /**
     * @brief Tokenizálja a fennmaradó megtisztított SAN kifejezést (standard algebraic notation)
     * 
     * Megkeresi a bábukat [KQRBN], oszlopokat [a-h], sorokat [1-8], elfogásokat (x), sakkokat (+), mattot (#), 
     * és a sáncolást (O-O vagy O-O-O)
     */
    std::regex sanRegex(R"([KQRBN]?[a-h]?[1-8]?x?[a-h][1-8](?:=[QRBN])?[\+#]?|O-O(?:-O)?)");

    auto words_begin = std::sregex_iterator(rawMoveText.begin(), rawMoveText.end(), sanRegex);
    auto words_end = std::sregex_iterator();

    for (std::sregex_iterator i = words_begin; i != words_end; i++) {
        std::string sanMove = i->str();

        // 1. make a move object from san

        // 2. make the move on the board

        // 3. save to parsed moves vector
    }

    return parsedMoves;
}

std::string generatePGN(const std::vector<Move>& history) {
    std::string PGNString = "";

    return PGNString;
}