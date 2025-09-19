#pragma once
#include <string>

class Log {
public:
    enum class Level // types of log with different levels (i added fatal for memory-scary warnings)
    { 
        TRACE, // 
        DEBUG, 
        INFO, 
        WARNING, 
        ERR, //ERROR for some reason, using ERROR gives an error while compiling, i assume it is a reserved-keyword thing
        FATAL 
    }; 

    static void initialize(Level consoleLevel = Level::INFO, const char* fileName = "game.log");
    static void shutdown();
    static void setConsoleLevel(Level level);
    static void log(Level level, const std::string& msg);
};
