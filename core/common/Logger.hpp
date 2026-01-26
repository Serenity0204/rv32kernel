#pragma once
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>

enum class LogLevel
{
    INFO,
    WARNING,
    ERROR,
    DEBUG
};

enum class LogSource
{
    KERNEL,
    SCHEDULER,
    SYSCALL,
    MMU,
    LOADER,
    CPU
};

class Logger
{
public:
    static Logger& getInstance()
    {
        static Logger instance;
        return instance;
    }

    // Main Log Function
    void log(LogSource source, LogLevel level, const std::string& message)
    {
        std::lock_guard<std::mutex> lock(this->logMutex);

        if (!this->logFile.is_open()) return;

        std::string sourceStr = "";

        // Map Source to String
        switch (source)
        {
        case LogSource::KERNEL:
            sourceStr = "[KERNEL   ]";
            break;
        case LogSource::SCHEDULER:
            sourceStr = "[SCHEDULER]";
            break;
        case LogSource::SYSCALL:
            sourceStr = "[SYSCALL  ]";
            break;
        case LogSource::MMU:
            sourceStr = "[MMU      ]";
            break;
        case LogSource::LOADER:
            sourceStr = "[LOADER   ]";
            break;
        case LogSource::CPU:
            sourceStr = "[CPU      ]";
            break;
        }

        if (level == LogLevel::ERROR)
        {
            this->logFile << sourceStr << " ERROR: " << message << std::endl;
            return;
        }
        if (level == LogLevel::WARNING)
        {
            this->logFile << sourceStr << " WARN: " << message << std::endl;
            return;
        }

        this->logFile << sourceStr << " " << message << std::endl;
    }

private:
    Logger()
    {
        // Open log.txt in truncate mode (clears the file every time you run)
        logFile.open("log.txt", std::ios::out | std::ios::trunc);
        if (!logFile)
        {
            std::cout << "Cannot open logfile, kernel crashed.\n";
            exit(1);
        }
    }
    ~Logger()
    {
        if (logFile.is_open())
            this->logFile.close();
    }
    std::mutex logMutex;
    std::ofstream logFile;
};

#define LOG(source, level, msg) Logger::getInstance().log(LogSource::source, LogLevel::level, msg)