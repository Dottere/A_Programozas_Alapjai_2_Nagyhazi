#pragma once

#include "chesstypes.hpp"
#include <string_view>

inline constexpr std::string_view DEFAULT_FEN =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

constexpr int BOARD_SIZE = 8;

namespace CASTLING_POSITION_CONSTANTS {
    inline constexpr int KING_DEST_KINGSIDE_X = 6;
    inline constexpr int KING_DEST_QUEENSIDE_X = 2;
    inline constexpr int ROOK_START_KINGSIDE_X = 7;
    inline constexpr int ROOK_START_QUEENSIDE_X = 0;
    inline constexpr int ROOK_DEST_KINGSIDE_X = 5;
    inline constexpr int ROOK_DEST_QUEENSIDE_X = 3;
}

enum class PIECE_VALUES {
    PAWN = 1,
    BISHOP = 3,
    KNIGHT = 3,
    ROOK = 5,
    QUEEN = 9,
    KING = 10
};