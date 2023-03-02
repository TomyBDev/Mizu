#include <mzpch.h>
#include <Tools/Log.h>

#include <spdlog/sinks/basic_file_sink.h>

std::shared_ptr<spdlog::logger> Log::logger;

void Log::Init()
{
	// Get local time to use in log filename.
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

	// Format file name using time.
	std::stringstream fileOutput;
	fileOutput << "../logs/MizuLog-" << std::put_time(&tm, "%Y.%m.%d-%H.%M.%S") << ".txt";
	// Setup the output log file.
	logger = spdlog::basic_logger_mt("Mizu", fileOutput.str());
	logger->set_pattern("[%H:%S][%l][%s line: %#]: %v");
	logger->set_level(spdlog::level::trace);
	set_default_logger(logger);
}