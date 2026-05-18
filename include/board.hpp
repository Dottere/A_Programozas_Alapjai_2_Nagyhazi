#pragma once

#include "chesstypes.hpp"
#include "piece.hpp"
#include "constants.hpp"

#include <memory>
#include <vector>
#include <array>
#include <string>
#include <string_view>
#include <optional>
#include <cstdint>

/*
 * Ez a fejlécfájl lesz felelős a tábla kezeléséért, a bábuk reprezentációjáért. A tábla önmagában egy 8x8-as tömb,
 * amely Bábu mutatókat tartalmaz. Ezen felül felelős lesz a PGN értelmezéséért.
 */

class Piece;

using Square = std::unique_ptr<Piece>;
using ChessBoard = std::array<std::array<Square, 8>, 8>;

/**
 * @brief Ez az osztály felelős a tábla állapotáért és a lépések validálásáért.
 *
 * A tábla osztály tartalmazza azt a 8x8 cellát amelyre a bábuk léphetnek, minden lépéskor szimulálja azt, majd ha az valid --
 * tehát nem ugrunk át csúszó bábukkal más bábukat, vagy nem ugrunk le a tábláról -- akkor megteszi azt.
 */
class Board
{
    ChessBoard board;

    std::vector<Square> whiteCaptured;
    std::vector<Square> blackCaptured;

    Color turn = Color::WHITE;

    castlingRights castlingRight;

    std::optional<Position<>> enPassantTarget;

    int halfMoveClock = 0;  // 50 lépéses szabály
    int fullMoveNumber = 1; // Teljes lépésszám

    Position<> whiteKingPos;
    Position<> blackKingPos;

    // handle coordinate translation in place
    [[nodiscard]] Square &at(Position<> pos) { return board[pos.x][7 - pos.y]; }

    [[nodiscard]] const Square &at(Position<> pos) const { return board[pos.x][7 - pos.y]; }

public:
    Board() = default;
    // Speciális másoló konstruktor az std::unique_ptr használata miatt
    Board(const Board &b);
    ~Board() = default;

    [[nodiscard]] Position<> getWhiteKingPos() const { return whiteKingPos; }
    [[nodiscard]] Position<> getBlackKingPos() const { return blackKingPos; }
    void setWhiteKingPos(Position<> pos) { whiteKingPos = pos; }
    void setBlackKingPos(Position<> pos) { blackKingPos = pos; }

    [[nodiscard]] int getHalfMoveClock() const { return halfMoveClock; }
    [[nodiscard]] int getFullMoveNumber() const { return fullMoveNumber; }
    void setHalfMoveClock(int val) { halfMoveClock = val; }
    void setFullMoveNumber(int val) { fullMoveNumber = val; }

    [[nodiscard]] const std::vector<Square> &getWhiteCaptured() const { return whiteCaptured; }
    [[nodiscard]] const std::vector<Square> &getBlackCaptured() const { return blackCaptured; }

    [[nodiscard]] bool isOnBoard(Position<> pos) const { return (pos.x >= 0 && pos.x <= 7 && pos.y >= 0 && pos.y <= 7); }

    [[nodiscard]] Piece *getPiece(Position<> pos) { return isOnBoard(pos) ? at(pos).get() : nullptr; }

    [[nodiscard]] const Piece *getPiece(Position<> pos) const { return isOnBoard(pos) ? at(pos).get() : nullptr; }

    [[nodiscard]] bool isPathClear(const std::vector<Position<>>& path) const;

    [[nodiscard]] bool isCheck(Color c);
    [[nodiscard]] bool hasLegalMoves(Color c);
    [[nodiscard]] bool isCheckMate(Color c);
    [[nodiscard]] bool isStaleMate(Color c);

    [[nodiscard]] bool placePiece(Square piece, Position<> pos);
    [[nodiscard]] std::optional<Position<>> findStartSquare(char pieceType, bool isWhiteToMove, Position<> endPos, char fileDisambiguity, char rankDisambiguity);

    [[nodiscard]] bool isWhiteToMove() const { return (turn == Color::WHITE); }
    [[nodiscard]] Color getTurn() const { return turn; }

    void nextTurn() { turn = (turn == Color::WHITE) ? Color::BLACK : Color::WHITE; }
    void clearBoard();

    bool loadFromFEN(std::string_view fen);

    void makeMove(Position<> startPos, Position<> endPos);

    [[nodiscard]] std::string generateFEN() const;

    void removePiece(Position<> pos);

    void setEnPassantTarget(Position<> target) { enPassantTarget = target; }
    [[nodiscard]]std::optional<Position<>> getEnPassantTarget() const { return enPassantTarget; }

    void promotePiece(Position<> pos, char promotedTo, Color pieceColor);

    void undoMove(const Move& m);

    void applyMove(const Move& m);
};