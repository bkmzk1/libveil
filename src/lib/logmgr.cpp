
#include <veil/logmgr.hpp>

namespace veil {

LogTimer::LogTimer(std::string_view name, std::source_location loc) {

    m_name = name;
    m_location = loc;
    m_startTimePoint = std::chrono::steady_clock::now();

    std::cout << Log::message(LogType::INFO, "Process '{}' started at '{}'", 
                              m_name, m_location.function_name()) << std::endl;
}

LogTimer::~LogTimer() {

    const std::chrono::steady_clock::time_point endTimePoint = std::chrono::steady_clock::now();
    const auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTimePoint - m_startTimePoint);

    std::cout << Log::message(LogType::INFO, "Process '{}' finished in {}", m_name, elapsedTime) << std::endl;
}

Exception::Exception(std::string_view msg, std::source_location loc) 
    : std::runtime_error(msg.data()), m_location(loc) {
    
    m_error_message = Log::message(LogType::CRITICAL, "{} in {}", msg, m_location.function_name());
}

const char* Exception::what() const noexcept {

    return m_error_message.c_str();
}

} //namespace veil