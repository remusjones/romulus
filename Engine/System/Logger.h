#pragma once
#include <spdlog/common.h>


#ifdef _MSC_VER
#define FUNCTION_SIGNATURE __FUNCSIG__
#else
#define FUNCTION_SIGNATURE __PRETTY_FUNCTION__
#endif

#define LOG_INFO(message) Logger::Log(fmt::format("{} {}:{} ", message, FUNCTION_SIGNATURE, __LINE__))
#define LOG_ERROR(message) Logger::LogError(fmt::format("{} {}:{} ", message, FUNCTION_SIGNATURE, __LINE__))
#define LOG_WARN(message) Logger::LogWarning(fmt::format("{} {}:{} ", message, FUNCTION_SIGNATURE, __LINE__))

#define LOG_INFO_VERBOSE(message) Logger::Log(fmt::format("{} {}:{}:{} ", message, __FILE__, FUNCTION_SIGNATURE, __LINE__))
#define LOG_ERROR_VERBOSE(message) Logger::LogError(fmt::format("{} {}:{}:{} ", message, __FILE__, FUNCTION_SIGNATURE, __LINE__))
#define LOG_WARN_VERBOSE(message) Logger::LogWarning(fmt::format("{} {}:{}:{} ", message, __FILE__, FUNCTION_SIGNATURE, __LINE__))

// todo: necessary class - we should just interface directly with spdlog
class Logger
{
public:
	// Placeholder until we get a proper log flow
	static void Log(spdlog::level::level_enum severity, const char* message);
	static void Log(spdlog::level::level_enum severity, const std::string& message);
	static void Log(const char* message);
	static void Log(const std::string& message);
	static void LogWarning(const std::string& message);
	static void LogError(const char* message);
	static void LogError(const std::string& message);
};
