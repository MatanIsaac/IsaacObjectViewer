/**
 * @brief Logging utilities.
 * This header file provides utilities for logging messages.
 * It uses the spdlog library for logging.
 */

#pragma once

#include <spdlog/spdlog.h>
#include <memory>
#include <iostream>
#include <string>

/// @brief Extracts the short file name from a file path.
/// @param filePath The full file path.
/// @return The short file name.
constexpr const char *shortFileName(const char *filePath)
{
    const char *file = filePath;
    for (const char *p = filePath; *p; ++p)
    {
        if (*p == '/' || *p == '\\')
        {
            file = p + 1;
        }
    }
    return file;
}

using SharedLogger = std::shared_ptr<spdlog::logger>;

class Log
{
public:
    /// @brief Initializes the logging system.
    static void Init();
    
    /**
     * @brief Get the shared logger instance.
     * @return A reference to the shared logger.
     */
    inline static SharedLogger &GetLogger()
    {
        if (!s_Logger)
            std::cout << "Logger is null, please call Log::Init() before Logging\n";

        return s_Logger;
    }

private:
    static SharedLogger s_Logger; // Declaration only
};

// Logging macros
#define LOG_TRACE(...) Log::GetLogger()->trace("[{}:{}]: {}", shortFileName(__FILE__), __LINE__, fmt::format(__VA_ARGS__))
#define LOG_DEBUG(...) Log::GetLogger()->debug("[{}:{}]: {}", shortFileName(__FILE__), __LINE__, fmt::format(__VA_ARGS__))
#define LOG_INFO(...) Log::GetLogger()->info("[{}:{}]: {}", shortFileName(__FILE__), __LINE__, fmt::format(__VA_ARGS__))
#define LOG_WARN(...) Log::GetLogger()->warn("[{}:{}]: {}", shortFileName(__FILE__), __LINE__, fmt::format(__VA_ARGS__))
#define LOG_ERROR(...) Log::GetLogger()->error("[{}:{}]: {}", shortFileName(__FILE__), __LINE__, fmt::format(__VA_ARGS__))
#define LOG_FATAL(...) Log::GetLogger()->critical("[{}:{}]: {}", shortFileName(__FILE__), __LINE__, fmt::format(__VA_ARGS__))
