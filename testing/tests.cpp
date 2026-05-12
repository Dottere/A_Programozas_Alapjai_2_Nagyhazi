#include <iostream>
#include "gtest_lite.h"
#include "piece.hpp"
#include "board.hpp"

int main() {
    GTINIT(std::cin); 

    TEST(FENTeszt, BetoltKiad) {
        constexpr std::string_view MID_GAME_POSITION = "r1bqk2r/pppp1ppp/2n2n2/4p3/2B1P3/2NP1N2/PPP2PPP/R1BQ1RK1 b kq - 4 6";
        Board board;
        board.loadFromFEN(MID_GAME_POSITION);
        EXPECT_EQ(MID_GAME_POSITION, board.generateFEN());
    } ENDM


    TEST(HuszarTeszt, SzabalyosLepesek) {
        Knight k(Color::WHITE);
        // L lépés
        EXPECT_TRUE(k.isValidMove(Position<>(4,4), Position<>(5,6), nullptr));
        // Egyenes lépés
        EXPECT_FALSE(k.isValidMove(Position<>(4,4), Position<>(4,5), nullptr));
    } ENDM 

    TEST(GyalogTeszt, FeherElore) {
        Pawn p(Color::WHITE);
        // Sima lépés előre
        EXPECT_TRUE(p.isValidMove(Position<>(4,1), Position<>(4,2), nullptr));
        // Ütés előre
        Piece* dummyEnemy = new Rook(Color::BLACK);
        EXPECT_FALSE(p.isValidMove(Position<>(4,1), Position<>(4,2), dummyEnemy));
        delete dummyEnemy;
    } ENDM 

    TEST(KiralynoSancolasTeszt, Kiraly) {
        King k(Color::WHITE);
        // Ha még nem lépett, érvényes
        EXPECT_TRUE(k.isValidMove(Position<>(4,0), Position<>(6,0), nullptr));
        
        // Ha már lépett, érvénytelen
        k.setHasMoved();
        EXPECT_FALSE(k.isValidMove(Position<>(4,0), Position<>(6,0), nullptr));
    } ENDM 

    GTEND(std::cerr); 

    return 0;
}