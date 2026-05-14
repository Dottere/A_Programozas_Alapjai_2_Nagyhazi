#pragma once

#include <iostream>
#include <string>

class Logger {
public:
    enum class Level { INFO, DEBUG, WARNING, ERROR};

    static void log(Level level, const std::string& file, int line, const std::string& message) {
        std::cout << "[" << levelToString(level) << "] [" << file << ":" << line << "] " << message << std::endl;  
    }

private:
    static std::string levelToString(Level level) {
        switch (level) {
            case Level::DEBUG:   return "DEBUG";
            case Level::INFO:    return "INFO";
            case Level::WARNING: return "WARN";
            case Level::ERROR:   return "ERR";
            default:             return "UNKNOWN";
        }
    }
};

#ifndef MDEBUG
    #define LOG_INFO(msg)
    #define LOG_DEBUG(msg)
    #define LOG_WARNING(msg)
    #define LOG_ERROR(msg)
#else
    #define LOG_INFO(msg) Logger::log(Logger::Level::INFO, __FILE__, __LINE__, msg)
    #define LOG_DEBUG(msg) Logger::log(Logger::Level::DEBUG, __FILE__, __LINE__, msg)
    #define LOG_WARNING(msg) Logger::log(Logger::Level::WARNING, __FILE__, __LINE__, msg)
    #define LOG_ERROR(msg) Logger::log(Logger::Level::ERROR, __FILE__, __LINE__, msg)
#endif
