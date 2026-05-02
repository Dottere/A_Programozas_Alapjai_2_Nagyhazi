#ifndef PIECE_HPP
#define PIECE_HPP

#include <string>

/**
 * @brief Egy szimpla enum amely nevesíti a bábuk színeit.
 */
enum class Color {
    WHITE,
    BLACK,
    NONE
};

/**
 * @brief Az olvashatóság és egyszerűség kedvéért egy az x és y koordinátákat egyesítő struktúra
 */
template <typename T = int>
struct Position{
    T x, y;

    Position(T x, T y) : x(x), y(y) {}

    template <typename U>
    bool operator==(const Position<U>& other) const {
        return x == other.x && y == other.y;
    }
};

/**
 * @brief A bábu alaposztály melyet kiterjeszt az összes többi bábu.
 */
class Piece {
public:
    Piece(Color color) {}
    virtual ~Piece() = default;

    // Dummy implementáció

    inline bool isKing() const { return false; }

    // Tisztán virtuális függvény amit felülírnak a bábuk, polimorfizmus

    virtual bool isValidMove(Position<> startPos, Position<> endPos, const Piece* TargetPiece) const = 0;
    virtual Piece* clone() const = 0;

    // --- Getterek és Setterek ---
    
    inline Color getColor() const { return Color::NONE; }
    
    inline bool getHasMoved() const { return false; }
    inline void setHasMoved() {}

    inline const std::string& getSymbol() const { 
        static std::string dummy = ""; 
        return dummy; 
    }

    inline char getPieceType() const { return ' '; }
    inline bool isWhite() const { return true; }

    virtual inline bool canJump() const { return false; }
};

// ==========================================
// Származtatott bábuk
// ==========================================

class Rook : public Piece {
public:
    Rook(Color color) : Piece(color) {}
    bool isValidMove(Position<> startPos, Position<> endPos, const Piece* TargetPiece) const override;
    Piece* clone() const override { return new Rook(*this); }
};

class Knight : public Piece {
public:
    Knight(Color color) : Piece(color) {}
    bool isValidMove(Position<> startPos, Position<> endPos, const Piece* TargetPiece) const override;
    Piece* clone() const override { return new Knight(*this); }
    inline bool canJump() const override { return true; }
};

class Bishop : public Piece {
public:
    Bishop(Color color) : Piece(color) {}
    bool isValidMove(Position<> startPos, Position<> endPos, const Piece* TargetPiece) const override;
    Piece* clone() const override { return new Bishop(*this); }
};

class Queen : public Piece {
public:
    Queen(Color color) : Piece(color) {}
    bool isValidMove(Position<> startPos, Position<> endPos, const Piece* TargetPiece) const override;
    Piece* clone() const override { return new Queen(*this); }
};

class King : public Piece {
public:
    King(Color color) : Piece(color) {}
    bool isValidMove(Position<> startPos, Position<> endPos, const Piece* TargetPiece) const override;
    Piece* clone() const override { return new King(*this); }
};

class Pawn : public Piece {
public:
    Pawn(Color color) : Piece(color) {}
    bool isValidMove(Position<> startPos, Position<> endPos, const Piece* TargetPiece) const override;
    Piece* clone() const override { return new Pawn(*this); }
};

#endif