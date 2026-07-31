
#pragma once

#include <veil_export.h>

#include <chrono>
#include <iostream>
#include <format>
#include <source_location>
#include <stdexcept>
#include <array>
#include <string_view>

#include "assets.hpp"

namespace veil {

enum class LogType : uint8_t {
    INFO,
    WARNING,
    CRITICAL,
    COUNT
};

struct VEIL_EXPORT Log : util::Singleton<Log> {

    friend class util::Singleton<Log>;

    public:
        ~Log() = default;
        template<typename... Args>
        static std::string message(LogType type, std::format_string<Args...> fmt, Args&&... args) {

            std::string formatted = std::format(fmt, std::forward<Args>(args)...);
            std::string msg = "VEIL::" + typeToString(type) + ": " + formatted;

            return msg;
        }

    private:
        Log() = default;
        constexpr static std::string typeToString(LogType type) {

            constexpr std::array<std::string, static_cast<size_t>(LogType::COUNT)> names = {
                "INFO",
                "WARNING",
                "CRITICAL"
            };
            auto index = static_cast<size_t>(type);
            if (index >= names.size())
                return "UNKNOWN";
            return names[index];
        }
        
}; //class Log

class VEIL_EXPORT LogTimer {
    public:
        LogTimer(std::string_view name, std::source_location loc = std::source_location::current());
        ~LogTimer();
    private:
        std::string m_name;
        std::source_location m_location;
        std::chrono::steady_clock::time_point m_startTimePoint;
}; //class LogTimer

class VEIL_EXPORT Exception : public std::runtime_error {
    public:
        Exception(std::string_view, std::source_location loc = std::source_location::current());

        const char* what() const noexcept override;
    private:
        std::string m_error_message;
        std::source_location m_location;
}; //class Exception

}; //namespace veil