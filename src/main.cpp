#include <iostream>
#include "renderer.hpp"
#include "board.hpp"

int main(void) {
    //std::cout << "Hello World!" << std::endl;
    
    Board board;
    Renderer renderer;

    renderer.display(board);
    return 0;
}