#ifndef PIECE_HPP
#define PIECE_HPP

/*
* Ez a fejlécfájl lesz felelős a bábu ősosztály és az abból származtatott összes többi bábu megvalósításával.
*/

enum class Color {
    WHITE,
    BLACK
};

class Piece {
    Color PieceColor;
    bool hasMoved = false;

    public:
        Piece(Color color) : PieceColor(color) {;}
        virtual ~Piece() = default;

        virtual bool isValidMove(int x_start, int y_start, int x_end, int y_end, const Piece* TargetPiece) const = 0;

        Color getColor(void) const;
        
        inline bool getHasMoved(void) const {return hasMoved;}
        inline void setHasMoved(void)       {hasMoved = true;}
};

class Rook : public Piece {
    public:
        Rook(Color color) : Piece(color) {;}

        bool isValidMove(int x_start, int y_start, int x_end, int y_end, const Piece* TargetPiece) const override;
};

class Knight : public Piece {
    public:
        Knight(Color color) : Piece(color) {;}

        bool isValidMove(int x_start, int y_start, int x_end, int y_end, const Piece* TargetPiece) const override;
};

class Bishop : public Piece {
    public:
        Bishop(Color color) : Piece(color) {;}

        bool isValidMove(int x_start, int y_start, int x_end, int y_end, const Piece* TargetPiece) const override;
};

class Queen : public Piece {
    public:
        Queen(Color color) : Piece(color) {;}

        bool isValidMove(int x_start, int y_start, int x_end, int y_end, const Piece* TargetPiece) const override;
};

class King : public Piece {
    public:
        King(Color color) : Piece(color) {;}

        bool isValidMove(int x_start, int y_start, int x_end, int y_end, const Piece* TargetPiece) const override;
};

class Pawn : public Piece {
    public:
        Pawn(Color color) : Piece(color) {;}

        bool isValidMove(int x_start, int y_start, int x_end, int y_end, const Piece* TargetPiece) const override;
};

#endif