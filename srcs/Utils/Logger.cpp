#include <Logger.hpp>

Logger::Logger() {}
void Logger::setLevel(const t_logger_level new_level) {
	min_level = new_level;
}
Logger::~Logger() {}

std::string	Logger::print_header(t_logger_level level) {
	static std::string	level_names[MUTE] = {"[DEBUG]", "[INFO]", "[WARNING]", "[ERROR]"};
	static std::string	level_colors[MUTE] = {BOLDWHITE, BOLDGREEN, BOLDYELLOW, BOLDRED};
	std::stringstream 	result;

	if (flags & DATE) {
		std::time_t now = std::time(NULL);
		std::string	time = std::ctime(&now);
		time.resize(time.length() - 1);
		result << time << " - ";
	}
	if (flags & LEVEL) {
		result << level_colors[level] << level_names[level] << RESET << ":\t";
	}
	return result.str();
}

t_logger_level		Logger::min_level = ERR;
t_logger_flags		Logger::flags = static_cast<t_logger_flags>(DATE|LEVEL);
Logger::Debug		Logger::debug;
Logger::Info		Logger::info;
Logger::Warning		Logger::warning;
Logger::Error		Logger::error;
Logger::Hidden		Logger::hide;