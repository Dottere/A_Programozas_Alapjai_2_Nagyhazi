#include <iostream>
#include "gtest_lite.h"
#include "piece.hpp"
#include "board.hpp"
#include "renderer.hpp"

int main() {
    GTINIT(std::cin); 

    TEST(BoardTest, LoadInitialFEN) {
        constexpr std::string_view STARTING_FEN_POSITION = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        Board board;
        board.loadFromFEN(STARTING_FEN_POSITION);
        EXPECT_EQ(STARTING_FEN_POSITION, board.generateFEN());
    } ENDM

    TEST(BoardTest, FENRejectsInvalidStrings) {
        Board board;
        // The last 4 fields are missing, namely castling, en passant, half move clock, full move number
        EXPECT_FALSE(board.loadFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w")); 
        // Added invalid character at poisition field --> [x]nbqkbnr
        EXPECT_FALSE(board.loadFromFEN("xnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")); 
    } ENDM

    TEST(BoardTest, PathIsClearUnobstructed) {
        Board board;
        

        // Empty board with just a rook and a target on the opposite side
        board.loadFromFEN("8/8/8/8/8/8/8/r6K w - - 0 1");

        #ifdef ALSO_RENDER
            Renderer renderer(board);
            renderer.display();
        #endif
        
        EXPECT_TRUE(board.isPathClear(Position<>{0, 0}, Position<>{7, 0})); // a1 to h1
    } ENDM

    TEST(BoardTest, PathIsBlocked) {
        Board board;
        // Rook on a1, Pawn on d1, King on h1
        board.loadFromFEN("8/8/8/8/8/8/8/r2N3K w - - 0 1");

        #ifdef ALSO_RENDER
            Renderer renderer(board);
            renderer.display();
        #endif

        EXPECT_FALSE(board.isPathClear(Position<>{0, 0}, Position<>{7, 0})); // a1 to h1 blocked by d1
    } ENDM

    TEST(BoardTest, DetectsCheck) {
        Board board;
        // Black king on e8 is attacked by White Queen on e2
        board.loadFromFEN("4k3/8/8/8/8/8/4Q3/4K3 b - - 0 1");

        #ifdef ALSO_RENDER
            Renderer renderer(board);
            renderer.display();
        #endif

        EXPECT_TRUE(board.isCheck(Color::BLACK));
        EXPECT_FALSE(board.isCheck(Color::WHITE));
    } ENDM

    TEST(BoardTest, DetectsCheckmate) {
        Board board;
        // Fool's Mate
        board.loadFromFEN("rnb1kbnr/pppp1ppp/8/4p3/6Pq/5P2/PPPPP2P/RNBQKBNR w KQkq - 1 3");
        #ifdef ALSO_RENDER
            Renderer renderer(board);
            renderer.display();
        #endif
        EXPECT_TRUE(board.isCheckMate(Color::WHITE));
        EXPECT_FALSE(board.hasLegalMoves(Color::WHITE));
    } ENDM

    TEST(BoardTest, DetectsStalemate) {
        Board board;
        // Famous stalemate position: Black king on a8, White king on c8, White queen on c7. Black to move.
        board.loadFromFEN("k1K5/2Q5/8/8/8/8/8/8 b - - 0 1");
        #ifdef ALSO_RENDER
            Renderer renderer(board);
            renderer.display();
        #endif
        EXPECT_FALSE(board.isCheck(Color::BLACK)); // Not in check
        EXPECT_TRUE(board.isStaleMate(Color::BLACK)); // But no legal moves
    } ENDM

    GTEND(std::cerr); 

    return 0;
}