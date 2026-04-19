#include "main.hpp"

#include <iostream>

#include "renderer.hpp"
#include "board.hpp"

int main(void) {   
    Board board;
    Renderer renderer(board);

    // hard coded testing xddd
    // maybe handle later via pgn

    Piece* white_rook1 = new Rook(Color::WHITE);
    Piece* white_horse1 = new Knight(Color::WHITE);
    Piece* white_bishop1 = new Bishop(Color::WHITE);
    Piece* white_rook2 = new Rook(Color::WHITE);
    Piece* white_horse2 = new Knight(Color::WHITE);
    Piece* white_bishop2 = new Bishop(Color::WHITE);
    Piece* white_queen = new Queen(Color::WHITE);
    Piece* white_king = new King(Color::WHITE);

    Piece* black_rook1 = new Rook(Color::BLACK);
    Piece* black_horse1 = new Knight(Color::BLACK);
    Piece* black_bishop1 = new Bishop(Color::BLACK);
    Piece* black_rook2 = new Rook(Color::BLACK);
    Piece* black_horse2 = new Knight(Color::BLACK);
    Piece* black_bishop2 = new Bishop(Color::BLACK);
    Piece* black_queen = new Queen(Color::BLACK);
    Piece* black_king = new King(Color::BLACK);

    board.placePiece(*white_rook1, Position(0,0));
    board.placePiece(*white_rook2, Position(7,0));
    board.placePiece(*white_horse1, Position(1,0));
    board.placePiece(*white_horse2, Position(6,0));
    board.placePiece(*white_bishop1, Position(2,0));
    board.placePiece(*white_bishop2, Position(5,0));
    board.placePiece(*white_queen, Position(3,0));
    board.placePiece(*white_king, Position(4,0));

    board.placePiece(*black_rook1, Position(0,7));
    board.placePiece(*black_rook2, Position(7,7));
    board.placePiece(*black_horse1, Position(1,7));
    board.placePiece(*black_horse2, Position(6,7));
    board.placePiece(*black_bishop1, Position(2,7));
    board.placePiece(*black_bishop2, Position(5,7));
    board.placePiece(*black_queen, Position(3,7));
    board.placePiece(*black_king, Position(4,7));

    for (int x = 0; x < 8; x++) {
        board.placePiece(*new Pawn(Color::WHITE), Position(x, 1));
    }

    for (int x = 0; x < 8; x++) {
        board.placePiece(*new Pawn(Color::BLACK), Position(x, 6));
    }

    renderer.display();
    return 0;
}