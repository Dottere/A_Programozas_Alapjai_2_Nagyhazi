#include <iostream>
#include "gtest_lite.h"
#include "piece.hpp"
#include "board.hpp"
#include "renderer.hpp"

int main()
{
    GTINIT(std::cin);

    TEST(BoardTest, LoadInitialFEN)
    {
        constexpr std::string_view STARTING_FEN_POSITION = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        Board board;
        board.loadFromFEN(STARTING_FEN_POSITION);
        EXPECT_EQ(STARTING_FEN_POSITION, board.generateFEN());
    }
    ENDM

    TEST(BoardTest, FENRejectsInvalidStrings)
    {
        Board board;
        // The last 4 fields are missing, namely castling, en passant, half move clock, full move number
        EXPECT_FALSE(board.loadFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w"));
        // Added invalid character at poisition field --> [x]nbqkbnr
        EXPECT_FALSE(board.loadFromFEN("xnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
    }
    ENDM

    TEST(BoardTest, FENHandlesEnPassantTarget)
    {
        Board board;
        // Standard starting position, but White just played e2-e4.
        // The en passant target square is now e3.
        constexpr std::string_view EP_FEN = "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1";

        board.loadFromFEN(EP_FEN);

#ifdef ALSO_RENDER
        Renderer renderer(board);
        renderer.display();
#endif

        // Check if the en passant target is still correct
        EXPECT_EQ(EP_FEN, board.generateFEN());
    }
    ENDM

    TEST(BoardTest, FENHandlesPartialCastlingRights)
    {
        Board board;
        // White can only castle Kingside, Black can only castle Queenside
        constexpr std::string_view PARTIAL_CASTLING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w Kq - 0 1";

        board.loadFromFEN(PARTIAL_CASTLING_FEN);

        EXPECT_EQ(PARTIAL_CASTLING_FEN, board.generateFEN());
    }
    ENDM

    TEST(BoardTest, FENHandlesNoCastlingRights)
    {
        Board board;
        // Neither side has castling rights (-)
        constexpr std::string_view NO_CASTLING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1";

        board.loadFromFEN(NO_CASTLING_FEN);
        EXPECT_EQ(NO_CASTLING_FEN, board.generateFEN());
    }
    ENDM

    TEST(BoardTest, PathIsClearUnobstructed)
    {
        Board board;

        // Empty board with just a rook and a target on the opposite side
        board.loadFromFEN("8/8/8/8/8/8/8/r6K w - - 0 1");

#ifdef ALSO_RENDER
        Renderer renderer(board);
        renderer.display();
#endif

        EXPECT_TRUE(board.isPathClear(Position<>{0, 0}, Position<>{7, 0})); // a1 to h1
    }
    ENDM

    TEST(BoardTest, DiagonalPathIsClearUnobstructed)
    {
        Board board;
        // White Bishop on a1, Black King on h8. The long diagonal is empty.
        board.loadFromFEN("7k/8/8/8/8/8/8/B7 w - - 0 1");

#ifdef ALSO_RENDER
        Renderer renderer(board);
        renderer.display();
#endif

        EXPECT_TRUE(board.isPathClear(Position<>{0, 0}, Position<>{7, 7})); // a1 to h8
    }
    ENDM

    TEST(BoardTest, PathIsBlocked)
    {
        Board board;
        // Rook on a1, Pawn on d1, King on h1
        board.loadFromFEN("8/8/8/8/8/8/8/r2N3K w - - 0 1");

#ifdef ALSO_RENDER
        Renderer renderer(board);
        renderer.display();
#endif

        EXPECT_FALSE(board.isPathClear(Position<>{0, 0}, Position<>{7, 0})); // a1 to h1 blocked by d1
    }
    ENDM

    TEST(BoardTest, DiagonalPathIsBlocked)
    {
        Board board;
        // White Bishop on a1, Black Pawn on d4, Black King on h8.
        board.loadFromFEN("7k/8/8/8/3p4/8/8/B7 w - - 0 1");

#ifdef ALSO_RENDER
        Renderer renderer(board);
        renderer.display();
#endif

        // The path from a1 to h8 should be blocked by the pawn on d4 (x: 3, y: 3)
        EXPECT_FALSE(board.isPathClear(Position<>{0, 0}, Position<>{7, 7}));
    }
    ENDM

    TEST(BoardTest, DetectsCheck)
    {
        Board board;
        // Black king on e8 is attacked by White Queen on e2
        board.loadFromFEN("4k3/8/8/8/8/8/4Q3/4K3 b - - 0 1");

#ifdef ALSO_RENDER
        Renderer renderer(board);
        renderer.display();
#endif

        EXPECT_TRUE(board.isCheck(Color::BLACK));
        EXPECT_FALSE(board.isCheck(Color::WHITE));
    }
    ENDM

    TEST(BoardTest, DetectsCheckmate)
    {
        Board board;
        // Fool's Mate
        board.loadFromFEN("rnb1kbnr/pppp1ppp/8/4p3/6Pq/5P2/PPPPP2P/RNBQKBNR w KQkq - 1 3");

#ifdef ALSO_RENDER
        Renderer renderer(board);
        renderer.display();
#endif

        EXPECT_TRUE(board.isCheckMate(Color::WHITE));
        EXPECT_FALSE(board.hasLegalMoves(Color::WHITE));
    }
    ENDM

    TEST(BoardTest, DetectsStalemate)
    {
        Board board;
        // Famous stalemate position
        board.loadFromFEN("k1K5/2Q5/8/8/8/8/8/8 b - - 0 1");

#ifdef ALSO_RENDER
        Renderer renderer(board);
        renderer.display();
#endif

        EXPECT_FALSE(board.isCheck(Color::BLACK));    // Not in check
        EXPECT_TRUE(board.isStaleMate(Color::BLACK)); // But no legal moves
    }
    ENDM

    TEST(BoardTest, FindStartSquareDisambiguation)
    {
        Board board;
        // Two white knights: one on b1, one on f3. Both can jump to d2.
        board.loadFromFEN("rnbqkbnr/pppppppp/8/8/8/5N2/PPP1PPPP/RNBQKB1R w KQkq - 0 1");

#ifdef ALSO_RENDER
        Renderer renderer(board);
        renderer.display();
#endif

        Position<> targetSquare(3, 1); // d2

        // Test: "Nbd2" -> File 'b' disambiguation
        auto startPos1 = board.findStartSquare('N', true, targetSquare, 'b', '\0');
        EXPECT_TRUE(startPos1.has_value());

        if (!startPos1.has_value())
        {
            EXPECT_EQ(startPos1->x, 1); // 'b' file
            EXPECT_EQ(startPos1->y, 0); // '1' rank
        }

        // Test: "Nfd2" -> File 'f' disambiguation
        auto startPos2 = board.findStartSquare('N', true, targetSquare, 'f', '\0');
        EXPECT_TRUE(startPos2.has_value());

        if (!startPos2.has_value())
        {
            EXPECT_EQ(startPos2->x, 5); // 'f' file
            EXPECT_EQ(startPos2->y, 2); // '3' rank
        }
    }
    ENDM

    TEST(BoardTest, ClearBoardRemovesPiecesAndResetsState)
    {
        Board board;
        board.loadFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

#ifdef ALSO_RENDER
        Renderer renderer(board);
        renderer.display();
#endif

        // Kings should be found
        EXPECT_TRUE(board.getPiece(Position<>{4, 0})->isKing());
        EXPECT_TRUE(board.getPiece(Position<>{4, 7})->isKing());

        board.clearBoard();

#ifdef ALSO_RENDER
        renderer.display();
#endif

        // FEN for a completely empty board
        EXPECT_EQ(board.generateFEN(), "8/8/8/8/8/8/8/8 w - - 0 1");

        // Kings should no longer be found
        EXPECT_TRUE(board.getPiece(Position<>{4, 0}) == nullptr);
        EXPECT_TRUE(board.getPiece(Position<>{4, 7}) == nullptr);
    }
    ENDM

        GTEND(std::cerr);

    return 0;
}