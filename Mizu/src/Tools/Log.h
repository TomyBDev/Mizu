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