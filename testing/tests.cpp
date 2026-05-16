#include "gtest_lite.h"
#include "piece.hpp"
#include "board.hpp"
#include "renderer.hpp"
#include "pgnhandler.hpp"
#include "gamemaster.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

namespace BoardTests {
        void testBoardFunctions() {
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

                auto path = board.getPiece(Position<>{0, 0})->getPath(Position<>{0, 0}, Position<>{7, 0});
                EXPECT_TRUE(board.isPathClear(path)); // a1 to h1
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

                auto path = board.getPiece(Position<>{0, 0})->getPath(Position<>{0, 0}, Position<>{7, 7});
                EXPECT_TRUE(board.isPathClear(path)); // a1 to h8
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
                
                auto path = board.getPiece(Position<>(0, 0))->getPath(Position<>{0, 0}, Position<>{7, 0});
                EXPECT_FALSE(board.isPathClear(path)); // a1 to h1 blocked by d1
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
                auto path = board.getPiece(Position<>{0, 0})->getPath(Position<>{0, 0}, Position<>{7, 7});
                EXPECT_FALSE(board.isPathClear(path));
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

                if (startPos1.has_value())
                {
                        EXPECT_EQ(startPos1->x, 1); // 'b' file
                        EXPECT_EQ(startPos1->y, 0); // '1' rank
                }

                // Test: "Nfd2" -> File 'f' disambiguation
                auto startPos2 = board.findStartSquare('N', true, targetSquare, 'f', '\0');
                EXPECT_TRUE(startPos2.has_value());

                if (startPos2.has_value())
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
                EXPECT_TRUE((Position<>{4, 0} == board.getWhiteKingPos() || Position<>{4, 0} == board.getBlackKingPos()));
                EXPECT_TRUE((Position<>{4, 7} == board.getWhiteKingPos() || Position<>{4, 7} == board.getBlackKingPos()));

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
        }
}

namespace PGNTests {

        struct TempPGNFile
        {
                std::filesystem::path filePath;
                
                TempPGNFile(const std::string& name, const std::string& content) : filePath(name)
                {
                        std::ofstream out(filePath);
                        out << content;
                        out.close();
                }

                ~TempPGNFile()
                {
                        if (std::filesystem::exists(filePath))
                        {
                                std::filesystem::remove(filePath);
                        }
                }
        };

        Board createStartBoard()
        {
                Board b;
                b.loadFromFEN(DEFAULT_FEN);
                return b;
        }

        void testPGNFunctions() 
        {
                TEST(PGNHandlerTest, ParseMetadataCorrectly)
                {
                        std::string pgnContent = 
                                "[Event \"FIDE World Cup 2023\"]\n"
                                "[Site \"Baku AZE\"]\n"
                                "[Date \"2023.08.10\"]\n"
                                "[Round \"3.2\"]\n"
                                "[White \"Carlsen, M.\"]\n"
                                "[Black \"Keymer, V.\"]\n"
                                "[Result \"1-0\"]\n"
                                "\n"
                                "1. e4 e5 2. Nf3 Nc6 1-0";

                                TempPGNFile tempFile("test_meta.pgn", pgnContent);
                                Board initialBoard = createStartBoard();

                                auto [metadata, moves] =
                                PGNHandler::parseFile(tempFile.filePath, initialBoard)
                                .value_or(std::make_pair(PGNMetadata{}, std::vector<Move>{}));

                                EXPECT_EQ(std::string("FIDE World Cup 2023"), metadata.event);
                                EXPECT_EQ(std::string("Baku AZE"), metadata.site);
                                EXPECT_EQ(std::string("2023.08.10"), metadata.date);
                                EXPECT_EQ(std::string("3.2"), metadata.round);
                                EXPECT_EQ(std::string("Carlsen, M."), metadata.white);
                                EXPECT_EQ(std::string("Keymer, V."), metadata.black);
                                EXPECT_EQ(std::string("1-0"), metadata.result);
                } 
                ENDM

                TEST(PGNHandlerTest, ParsesBasicMoves)
                {
                        // Scholar's mate
                        TempPGNFile tempFile("test_basic.pgn", "1. e4 e5 2. Bc4 Nc6 3. Qh5 Nf6 4. Qxf7# 1-0");
                        Board initialBoard = createStartBoard();

                        auto [metadata, moves] = 
                        PGNHandler::parseFile(tempFile.filePath, initialBoard)
                        .value_or(std::make_pair(PGNMetadata{}, std::vector<Move>{}));

                        EXPECT_EQ(7u, moves.size()); // 4 white moves, 3 black moves

                        // Check White's first move: e4
                        EXPECT_EQ('P', moves[0].movedPiece);
                        EXPECT_EQ(4, moves[0].endPos.x); // 'e' file (assuming a=0, b=1, ..., e=4)
                        EXPECT_EQ(3, moves[0].endPos.y); // 4th rank (assuming 1st=0, ..., 4th=3)

                        // Check Black's first move: e5
                        EXPECT_EQ('P', moves[1].movedPiece);
                        EXPECT_EQ(4, moves[1].endPos.x); 
                        EXPECT_EQ(4, moves[1].endPos.y); 

                        // Check the final capture and checkmate flag: Qxf7#
                        EXPECT_TRUE(moves[6].flags.isCapture);
                        EXPECT_TRUE(moves[6].flags.isCheckMate);
                        EXPECT_EQ('Q', moves[6].movedPiece);
                }
                ENDM

                TEST(PGNHandlerTest, ParsesSpecialMoves) 
                {
                        // A fabricated game line that forces castling and promotion
                        std::string movesText = "1. e4 d5 2. exd5 Nf6 3. d4 Nxd5 4. Nf3 Nc6 5. Be2 Bg4 6. O-O e5 7. dxe5 f6 8. exf6 Qd7 9. fxg7 O-O-O 10. gxh8=Q";
                        TempPGNFile tempFile("test_special.pgn", movesText);
                        Board initialBoard = createStartBoard();

                        auto [metadata, moves] = 
                        PGNHandler::parseFile(tempFile.filePath, initialBoard)
                        .value_or(std::make_pair(PGNMetadata{}, std::vector<Move>{}));

                        EXPECT_EQ(19u, moves.size());

                        // White Kingside Castle (6. O-O) -> index 10
                        EXPECT_TRUE(moves[10].flags.isCastle);
                        EXPECT_EQ(6, moves[10].endPos.x); // Kingside

                        // Black Queenside Castle (9... O-O-O) -> index 17
                        EXPECT_TRUE(moves[17].flags.isCastle);
                        EXPECT_EQ(2, moves[17].endPos.x); // Queenside

                        // White Promotion (10. gxh8=Q) -> index 18
                        EXPECT_TRUE(moves[18].flags.isCapture);
                        EXPECT_EQ('Q', moves[18].promotedTo);
                }
                ENDM

                TEST(PGNHandlerTest, HandlesMissingFileGracefully) 
                {
                        Board initialBoard = createStartBoard();
                        auto [metadata, moves] = 
                        PGNHandler::parseFile("does_not_exist_ever.pgn", initialBoard)
                        .value_or(std::make_pair(PGNMetadata{}, std::vector<Move>{}));

                        // Should return empty data without crashing
                        EXPECT_TRUE(moves.empty());
                        EXPECT_TRUE(metadata.event.empty());
                }
                ENDM

                TEST(PGNHandlerTest, GeneratesCorrectPGNString) 
                {
                        PGNMetadata meta;
                        meta.white = "Player A";
                        meta.black = "Player B";
                        meta.result = "1/2-1/2";

                        // Manually construct a short history: 1. e4 e5
                        std::vector<Move> history;
                        
                        Move m1{{4, 1}, {4, 3}, {}, 'P', '\0', nullptr, {}, {}};
                        history.push_back(m1);

                        Move m2{{4, 6}, {4, 4}, {}, 'P', '\0', nullptr, {}, {}};
                        history.push_back(m2);

                        std::string generated = PGNHandler::generatePGN(meta, history);

                        // std::string::find returns std::string::npos if the text isn't found
                        EXPECT_TRUE(generated.find("[White \"Player A\"]") != std::string::npos);
                        EXPECT_TRUE(generated.find("[Black \"Player B\"]") != std::string::npos);
                        EXPECT_TRUE(generated.find("[Result \"1/2-1/2\"]") != std::string::npos);
                        EXPECT_TRUE(generated.find("1. e4 e5 1/2-1/2") != std::string::npos);
                }
                ENDM
        }
}

namespace GameMasterTests {
    void testGameMasterFunctions()
    {

        TEST(GameMasterTest, ConstructGameMasterSuccessfully)
        {
            Board board;
            Renderer renderer(board);
            
            // UPDATED: Constructed GameMaster using your cleaned up parameter list 
            // (no longer attempts to decode time tracking configurations).
            [[maybe_unused]] GameMaster gm(board, renderer);
            
            EXPECT_TRUE(true);
        }
        ENDM

        TEST(GameMasterTest, ExecutingRunWithInvalidFilesDoesNotCrash)
        {
            Board board;
            Renderer renderer(board);
            GameMaster gm(board, renderer);

            gm.run(std::string(DEFAULT_FEN), "non_existent_game_flow.pgn");

            EXPECT_TRUE(true);
        }
        ENDM
    }
}

int main()
{
        GTINIT(std::cin);

        BoardTests::testBoardFunctions();
        PGNTests::testPGNFunctions();
        GameMasterTests::testGameMasterFunctions();

        GTEND(std::cerr);

        return 0;
}