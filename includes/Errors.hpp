#ifndef ERRORS_HPP
#define ERRORS_HPP

#include <Type.hpp>
#include <iostream>

class Errors {
	public:
		typedef std::map<uint, std::string>	t_error_map;
		Errors();
		Errors(const Errors& from);
		~Errors();
		Errors&				operator=(const Errors& rhs);
		void				addError(uint status_code, std::string path);
		std::string			getError(uint status_code) const;
		static std::string	getDefaultError(uint status_code);
		std::string&		operator[](uint http_error);
		t_error_map			getCustomErrorPath() const;

	private:
		t_error_map			custom_errors_path;
};

std::ostream&	operator<<(std::ostream &out, const Errors &errors);

#endif
