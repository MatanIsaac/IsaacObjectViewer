#include "Log.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>

// Definition of the static member
SharedLogger Log::s_Logger = nullptr;

void Log::Init()
{
    if (!s_Logger)
    {
        // Set the log pattern
        spdlog::set_pattern("[%T] [%l] %n: %v");

        // Attempt to retrieve an existing logger named "console"
        s_Logger = spdlog::get("console");
        if (!s_Logger)
        {
            // Create a new logger if it doesn't exist
            s_Logger = spdlog::stdout_color_mt("console");
        }

        s_Logger->set_level(spdlog::level::trace); // Set the logging level
    }
}
