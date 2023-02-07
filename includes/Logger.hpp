#pragma once
#ifndef LOGGER_HPP
# define LOGGER_HPP

#include <sstream>
#include <string>
#include <iostream>
#include <ctime>

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

typedef	enum {
	DEBUG,
	INFO,
	WARNING,
	ERROR,
	MUTE
}	t_logger_level;

typedef enum
{
	DATE =		0b001,
	LEVEL = 	0b010,
} t_logger_flags;


class Logger {
private:
	class Debug {
		public:
			Debug() {};
			~Debug() {};

			std::ostream&	operator<<(const std::string& message) {
				if (DEBUG < Logger::min_level)
					return hide;
				std::cout << print_header(DEBUG) << message;
				return std::cout;
			}
	};
	class Info {
		public:
			Info() {};
			~Info() {};

			std::ostream&	operator<<(const std::string& message) {
				if (INFO < Logger::min_level)
					return hide;
				std::cout << print_header(INFO) << message;
				return std::cout;
			}
	};
	class Warning {
		public:
			Warning() {};
			~Warning() {};

			std::ostream&	operator<<(const std::string& message) {
				if (WARNING < min_level)
					return hide;
				std::cerr << print_header(WARNING) << message;
				return std::cout;
			}
	};
	class Error {
		public:
			Error() {};
			~Error() {};

			std::ostream&	operator<<(const std::string& message) {
				if (ERROR < min_level)
					return hide;
				std::cerr << print_header(ERROR) << message;
				return std::cout;
			}
	};
	class Hidden: public std::ostream {
		public:
			Hidden() {};
			~Hidden() {};

			std::ostream&	operator<<(const std::string& message) {
				(void)message;
				return *this;
			}
	};


	static t_logger_level	min_level;
	static t_logger_flags	flags;
	static Hidden			hide;

	static std::string	print_header(t_logger_level level);

public:
	Logger();
	~Logger();

	static Debug	debug;
	static Info		info;
	static Warning	warning;
	static Error	error;

	static void setLevel(const t_logger_level new_level);
};

#endif
