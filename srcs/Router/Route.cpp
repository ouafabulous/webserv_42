#include "Router.hpp"

Route::Route(const t_attributes attributes)
: attributes(attributes) {

}
Route::Route(const Route& from)
: attributes(from.attributes) {}
Route&	Route::operator=(const Route& rhs) {
	// self-assignment guard
	if (this == &rhs)
		return *this;
	attributes = rhs.attributes;
	return *this;
}
Route::~Route() {

}

std::string	Route::getError(uint http_error) const {
	return attributes.error_files.getError(http_error);
}

size_t	Route::getMaxBodySize() const {
	return attributes.max_body_length;
}
void		Route::printAttributes() const {
	std::cout << "-----Methods allowed-----";
	if (attributes.allowed_methods & GET) {
		std::cout << "GET ";
	}
	else if (attributes.allowed_methods & POST) {
		std::cout << "POST ";
	}
	else if (attributes.allowed_methods & DELETE){
		std::cout << "DELETE";
	}
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "-----Server Names-----" << std::endl;
	std::cout << attributes.server_name << std::endl;
	// std::vector<std::string>::const_iterator	str_iter;
	// for (str_iter = attributes.server_names.begin(); str_iter != attributes.server_names.end(); ++str_iter){
		// std::cout << *str_iter << " ";
	// }
		// std::cout << std::endl;
	std::cout << std::endl;

	std::cout << "-----Location-----" << std::endl;
	std::cout << attributes.location << std::endl;
	std::cout << std::endl;
	std::cout << "-----Max body length-----" << std::endl;
	std::cout << attributes.max_body_length << std::endl;
	std::cout << std::endl;
	std::cout << "-----Redirect-----" << std::endl;
	std::cout << attributes.redirect << std::endl;
	std::cout << std::endl;
	std::cout << "-----Root-----" << std::endl;
	std::cout << attributes.root << std::endl;
	std::cout << std::endl;
	std::cout << "-----Directory lising-----" << std::endl;
	std::cout << attributes.directory_listing << std::endl;
	std::cout << std::endl;

}
