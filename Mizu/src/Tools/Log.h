#pragma once

namespace spdlog
{
	class logger;
}

class Log
{
public:
	static void Init();

	static std::shared_ptr<spdlog::logger>& GetLogger() { return logger; }

private:
	static std::shared_ptr<spdlog::logger> logger;
};

#ifdef MZ_DEBUG
#define LOG_TRACE(...) ::Log::GetLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...) ::Log::GetLogger()->info(__VA_ARGS__)
#define LOG_WARN(...) ::Log::GetLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) ::Log::GetLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::Log::GetLogger()->critical(__VA_ARGS__)
#define LOG_FLUSH() ::Log::GetLogger()->flush()
#else
#define LOG_TRACE()
#define LOG_INFO()
#define LOG_WARN()
#define LOG_ERROR()
#define LOG_CRITICAL()
#define LOG_FLUSH()
#endif

