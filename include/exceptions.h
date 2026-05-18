#pragma once

#include <stdexcept>
#include <string>

namespace ChessExcept
{
    class InvalidMoveException : public std::logic_error
    {
    public:
        InvalidMoveException(const std::string& message) : std::logic_error(message) {}
    };

    class MissingResourceException : public std::runtime_error
    {
    public:
        MissingResourceException(const std::string& message) : std::runtime_error(message) {}
    };

    class InvalidCastlingException : public InvalidMoveException
    {
    public:
        InvalidCastlingException(const std::string& message) : InvalidMoveException(message) {}
    };

    class InCheckException : public InvalidMoveException
    {
    public:
        InCheckException(const std::string& message) : InvalidMoveException(message) {}
    };

    class NonexistentPGNFileException : public MissingResourceException
    {
    public:
        NonexistentPGNFileException(const std::string& message) : MissingResourceException(message) {}
    };

    class NonexistentFENFileException : public MissingResourceException
    {
    public:
        NonexistentFENFileException(const std::string& message) : MissingResourceException(message) {}
    };

    class NonexistentPiecePointerException : public MissingResourceException
    {
    public:
        NonexistentPiecePointerException(const std::string& message) : MissingResourceException(message) {}
    };
}