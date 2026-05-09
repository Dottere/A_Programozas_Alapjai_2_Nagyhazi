#ifndef PIECE_HPP
#define PIECE_HPP

/*
* Ez a fejlécfájl lesz felelős a bábu ősosztály és az abból származtatott összes többi bábu megvalósításával.
*/

#include "chesstypes.hpp"
#include <string>
#include <memory>


// forward declaration
class Board;

/**
 * @brief A bábu alaposztály melyet kiterjeszt az összes többi bábu.
 * 
 * A bábuk tudják önmagukról, hogy hogyan léphetnek, milyen színűek, léptek-e már. Ezt kezeli ez az osztály.
 */
class Piece {
    Color PieceColor;
    /// Jelzi, hogy a bábu lépett-e már. (Sáncoláshoz, paraszt dupla lépéséhez fontos)
    bool hasMoved = false;

    protected: 
        std::string symbol;
        char pieceType;

    public:
        Piece(Color color) : PieceColor(color) {;}
        virtual ~Piece() = default;

        bool isKing() const { return pieceType == 'K'; }

        /**
         * @brief Megmondja, hogy az adott mezőre lépés lehetséges-e.
         * 
         * Ez a bábuk szíve, ez egy tisztán virtuális függvény amelyet minden bábunak kötelező felül írnia. 
         * Bekér egy kezdő és végső koordinátát, meg azt, hogy milyen bábu áll a végső koordinátán. Ha a bábu saját
         * szabályainak megfelel mindegyik paraméter, akkor igazat ad vissza, ellenben nem.
         * 
         * @param x_start A kezdő x koordináta
         * @param y_start A kezdő y koordináta
         * @param x_end A végső x koordináta
         * @param y_end A végső y koordináta
         * @param TargetPiece A végső mezőn álló bábura mutató, ha nem áll ott semmi akkor nullptr
         * 
         * @return True/Igaz ha a bábu szabályainak megfelelnek a paraméterek, ellenben False/Hamis
        */
        virtual bool isValidMove(Position<> startPos, Position<> endPos, const Piece* TargetPiece) const = 0;
        inline virtual std::unique_ptr<Piece> clone() const = 0;

        /**
         * @brief Visszaadja a bábu színét
         * @return A bábu színe
         */
        inline Color getColor() const {
            return PieceColor;  
        }
        
        /**
         * @brief Visszaadja, hogy a bábu lépett-e már.
         */
        inline bool getHasMoved() const {return hasMoved;}
        /**
         * @brief Igazra állítja a bábu azon változóját, amely számon tartja, hogy lépett-e már.
         */
        inline void setHasMoved()       {hasMoved = true;}

        inline const std::string& getSymbol() const { 
            return symbol; 
        }

        inline char getPieceType() const {
            return pieceType;
        }

        inline bool isWhite() const {
            return (PieceColor == Color::WHITE);
        }

        virtual inline bool canJump() const {
            return false;
        }
};

class Rook : public Piece {
    public:
        Rook(Color color) : Piece(color) {
            symbol = (color == Color::WHITE) ? "\u2656" : "\u265C";
            pieceType = 'R';
        }

        /** 
         * @brief A Bástya osztály lépésvalidáló metódusa
         * 
         * A bástya úgy léphet, ha csakis egyvonalban mozog, de ott bármennyit. Ezt úgy valósítja meg, hogy megnézi mely
         * koordináták változtak. Ha csak az x, vagy csak az y koordináták változtak, akkor a lépés helyes.
         * 
         * @param x_start A kezdő x koordináta
         * @param y_start A kezdő y koordináta
         * @param x_end A végső x koordináta
         * @param y_end A végső y koordináta
         * @param TargetPiece A végső mezőn álló bábura mutató, ha nem áll ott semmi akkor nullptr
         * @return True/Igaz ha a bábu szabályainak megfelelnek a paraméterek, ellenben False/Hamis
        */ 
        bool isValidMove(Position<> startPos, Position<> endPos, const Piece* TargetPiece) const override;

        inline std::unique_ptr<Piece> clone() const override {
            return std::make_unique<Rook>(*this);
        }
};

class Knight : public Piece {
    public:
        Knight(Color color) : Piece(color) {
            symbol = (color == Color::WHITE) ? "\u2658" : "\u265E";
            pieceType = 'N';
        }

        bool isValidMove(Position<> startPos, Position<> endPos, const Piece* TargetPiece) const override;

        inline std::unique_ptr<Piece> clone() const override {
            return std::make_unique<Knight>(*this);
        }

        inline bool canJump() const override {
            return true;
        }
};

class Bishop : public Piece {
    public:
        Bishop(Color color) : Piece(color) {
            symbol = (color == Color::WHITE) ? "\u2657" : "\u265D";
            pieceType = 'B';
        }

        bool isValidMove(Position<> startPos, Position<> endPos, const Piece* TargetPiece) const override;

        inline std::unique_ptr<Piece> clone() const override {
            return std::make_unique<Bishop>(*this);
        }
};

class Queen : public Piece {
    public:
        Queen(Color color) : Piece(color) {
            symbol = (color == Color::WHITE) ? "\u2655" : "\u265B";
            pieceType = 'Q';
        }

        bool isValidMove(Position<> startPos, Position<> endPos, const Piece* TargetPiece) const override;

        inline std::unique_ptr<Piece> clone() const override {
            return std::make_unique<Queen>(*this);
        }
};

class King : public Piece {
    public:
        King(Color color) : Piece(color) {
            symbol = (color == Color::WHITE) ? "\u2654" : "\u265A";
            pieceType = 'K';
        }

        bool isValidMove(Position<> startPos, Position<> endPos, const Piece* TargetPiece) const override;

        inline std::unique_ptr<Piece> clone() const override {
            return std::make_unique<King>(*this);
        }
};

class Pawn : public Piece {
    public:
        Pawn(Color color) : Piece(color) {
            symbol = (color == Color::WHITE) ? "\u2659" : "\u265F"; // \u2659
            pieceType = 'P';
        }

        bool isValidMove(Position<> startPos, Position<> endPos, const Piece* TargetPiece) const override;
        
        inline std::unique_ptr<Piece> clone() const override {
            return std::make_unique<Pawn>(*this);
        }
};

#endif