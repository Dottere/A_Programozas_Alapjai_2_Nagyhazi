#include "include/board.hpp"
#include <cmath>

Board::Board() {
    for (int i = 0; i < 8; i++){

        for (int j = 0; j < 8; j++) {
            board[i][j] = nullptr;
        }

    }
}

Board::~Board() {
        for (int i = 0; i < 8; i++){

        for (int j = 0; j < 8; j++) {
            if (board[i][j] != nullptr) delete board[i][j];
        }

    }
}

bool Board::isPathClear(int x_start, int y_start, int x_end, int y_end) const {
    int dx = std::abs(x_end - x_start);
    int dy = std::abs(y_end - y_start);
}