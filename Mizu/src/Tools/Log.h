#pragma once

#include <spdlog/spdlog.h>

class Log
{
public:
	static void Init();

	static std::shared_ptr<spdlog::logger>& GetLogger() { return logger; }

private:
	static std::shared_ptr<spdlog::logger> logger;
};

#ifdef MZ_DEBUG
#define LOG_TRACE(...) SPDLOG_TRACE(__VA_ARGS__)
#define LOG_INFO(...) SPDLOG_INFO(__VA_ARGS__)
#define LOG_WARN(...) SPDLOG_WARN(__VA_ARGS__)
#define LOG_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
#define LOG_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__)
#define LOG_FLUSH() ::Log::GetLogger()->flush()
#define LOG_INIT() ::Log::Init()
#else
#define LOG_TRACE()
#define LOG_INFO()
#define LOG_WARN()
#define LOG_ERROR()
#define LOG_CRITICAL()
#define LOG_FLUSH()
#define LOG_INIT()
#endif

