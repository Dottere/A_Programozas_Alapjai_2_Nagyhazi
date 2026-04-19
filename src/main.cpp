#include "main.hpp"

#include <iostream>

#include "renderer.hpp"
#include "board.hpp"

int main(void) {   
    Board board;
    Renderer renderer(board);

    // hard coded testing xddd

    Piece* white_rook1 = new Rook(Color::WHITE);
    Piece* white_horse1 = new Knight(Color::WHITE);
    Piece* white_bishop1 = new Bishop(Color::WHITE);
    Piece* white_rook2 = new Rook(Color::WHITE);
    Piece* white_horse2 = new Knight(Color::WHITE);
    Piece* white_bishop2 = new Bishop(Color::WHITE);
    Piece* white_queen = new Queen(Color::WHITE);
    Piece* white_king = new King(Color::WHITE);

    board.placePiece(*white_rook1, Position(0,0));
    board.placePiece(*white_rook2, Position(7,0));
    board.placePiece(*white_horse1, Position(1,0));
    board.placePiece(*white_horse2, Position(6,0));
    board.placePiece(*white_bishop1, Position(2,0));
    board.placePiece(*white_bishop2, Position(5,0));
    board.placePiece(*white_queen, Position(3,0));
    board.placePiece(*white_king, Position(4,0));

    for (int x = 0; x < 8; x++) {
        board.placePiece(*new Pawn(Color::WHITE), Position(x, 1));
    }

    for (int x = 0; x < 8; x++) {
        board.placePiece(*new Pawn(Color::BLACK), Position(x, 6));
    }

    renderer.display();
    return 0;
}