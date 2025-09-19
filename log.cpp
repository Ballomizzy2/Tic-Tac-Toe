#include "Log.h"
#include <fstream>
#include <mutex>
#include <sstream>
#include <chrono>
#include <iomanip>

#ifdef _WIN32
  #include <windows.h>
#endif

namespace {
    std::ofstream s_file;
    Log::Level    s_consoleLevel = Log::Level::INFO;
    std::mutex    s_mutex;

    const char* levelStr(Log::Level l) {
        switch (l) {
            case Log::Level::TRACE:   
                return "TRACE";
            case Log::Level::DEBUG:   
                return "DEBUG";
            case Log::Level::INFO:    
                return "INFO";
            case Log::Level::WARNING: 
                return "WARNING";
            case Log::Level::ERR:   
                return "ERROR";
            case Log::Level::FATAL:   
                return "FATAL";
        }
        return "";
    }

    std::string stamp() { // handles logging when the rime of the log came in to the document for tracking purposes
        using namespace std::chrono;
        auto now = system_clock::now();
        std::time_t t = system_clock::to_time_t(now);
        std::tm tm{};
        #ifdef _WIN32
            localtime_s(&tm, &t);
        #else
            localtime_r(&t, &tm);
        #endif
        std::ostringstream os;
        os << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return os.str();
    }

    void toDebugger(const std::string& s) {
        #ifdef _WIN32
            OutputDebugStringA(s.c_str());
        #else
            std::clog << s;
        #endif
    }
}

void Log::initialize(Level consoleLevel, const char* fileName) {
    std::lock_guard<std::mutex> g(s_mutex);
    s_consoleLevel = consoleLevel;
    s_file.open(fileName, std::ios::out | std::ios::trunc);

    // Always show this line regardless of console level:
    if (s_file.is_open()) { s_file << "Logging Initialized\n"; s_file.flush(); }
    toDebugger("Logging Initialized\n");
}

void Log::shutdown() {
    std::lock_guard<std::mutex> g(s_mutex);
    if (s_file.is_open()) { s_file.flush(); s_file.close(); }
}

void Log::setConsoleLevel(Level level) {
    std::lock_guard<std::mutex> g(s_mutex);
    s_consoleLevel = level;
}

void Log::log(Level level, const std::string& msg) {
    std::lock_guard<std::mutex> g(s_mutex);
    std::ostringstream line;
    line << stamp() << " [" << levelStr(level) << "] " << msg << "\n";
    const std::string s = line.str();

    // File: always record everything
    if (s_file.is_open()) { s_file << s; s_file.flush(); }

    // Console: respect current threshold
    if (static_cast<int>(level) >= static_cast<int>(s_consoleLevel)) {
        toDebugger(s);
    }
}
