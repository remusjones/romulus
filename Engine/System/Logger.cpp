#include "Logger.h"
#include <iostream>

#include <spdlog/spdlog.h>

void Logger::Log(spdlog::level::level_enum severity, const char* message)
{
	spdlog::log(severity, message);
}

void Logger::Log(const spdlog::level::level_enum severity, const std::string& message)
{
	spdlog::log(severity, message.c_str());
}

void Logger::Log(const char* message)
{
	Log(spdlog::level::info, message);
}

void Logger::Log(const std::string& message)
{
	Log(spdlog::level::info, message);
}

void Logger::LogWarning(const std::string& message)
{
	Log(spdlog::level::warn, message);
}

void Logger::LogError(const char* message)
{
	Log(spdlog::level::err, message);
}

void Logger::LogError(const std::string& message)
{
	Log(spdlog::level::err, message);
}
