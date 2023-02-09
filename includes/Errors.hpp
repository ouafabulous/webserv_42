#pragma once
#ifndef ERRORS_HPP
#define ERRORS_HPP

#include <Type.hpp>
#include <iostream>

class Errors {
	public:
		Errors();
		Errors(const Errors& from);
		~Errors();
		Errors&				operator=(const Errors& rhs);
		void				addError(uint status_code, std::string path);
		std::string			getError(uint status_code) const;
		static std::string	getDefaultError(uint status_code);

	private:
		std::map<uint, std::string>			custom_errors_path;
};

#endif
