#ifndef PIECE_HPP
#define PIECE_HPP

/*
* Ez a fejlécfájl lesz felelős a bábu ősosztály és az abból származtatott összes többi bábu megvalósításával.
*/

/**
 * @brief Egy szimpla enum amely nevesíti a bábuk színeit.
 */
enum class Color {
    WHITE,
    BLACK
};

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

    public:
        Piece(Color color) : PieceColor(color) {;}
        virtual ~Piece() = default;

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
        virtual bool isValidMove(int x_start, int y_start, int x_end, int y_end, const Piece* TargetPiece) const = 0;

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
};

class Rook : public Piece {
    public:
        Rook(Color color) : Piece(color) {
            symbol = (color == Color::WHITE) ? "\u2656" : "\u265C"; 
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
        bool isValidMove(int x_start, int y_start, int x_end, int y_end, const Piece* TargetPiece) const override;
};

class Knight : public Piece {
    public:
        Knight(Color color) : Piece(color) {
            symbol = (color == Color::WHITE) ? "\u2658" : "\u265E"; 
        }

        bool isValidMove(int x_start, int y_start, int x_end, int y_end, const Piece* TargetPiece) const override;
};

class Bishop : public Piece {
    public:
        Bishop(Color color) : Piece(color) {
            symbol = (color == Color::WHITE) ? "\u2657" : "\u265D";
        }

        bool isValidMove(int x_start, int y_start, int x_end, int y_end, const Piece* TargetPiece) const override;
};

class Queen : public Piece {
    public:
        Queen(Color color) : Piece(color) {
            symbol = (color == Color::WHITE) ? "\u2655" : "\u265B";
        }

        bool isValidMove(int x_start, int y_start, int x_end, int y_end, const Piece* TargetPiece) const override;
};

class King : public Piece {
    public:
        King(Color color) : Piece(color) {
            symbol = (color == Color::WHITE) ? "\u2654" : "\u265A";
        }

        bool isValidMove(int x_start, int y_start, int x_end, int y_end, const Piece* TargetPiece) const override;
};

class Pawn : public Piece {
    public:
        Pawn(Color color) : Piece(color) {
            symbol = (color == Color::WHITE) ? "\u2659" : "\u265F";
        }

        bool isValidMove(int x_start, int y_start, int x_end, int y_end, const Piece* TargetPiece) const override;
};

#endif