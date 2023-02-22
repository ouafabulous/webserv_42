#include "Router.hpp"
#include "Type.hpp"

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
	std::cout << "-----Port-----" << std::endl;
	std::cout << attributes.port << std::endl;
	std::cout << "-----Methods allowed-----" << std::endl;
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
	std::cout << "-----Server Names-----" << std::endl;
	std::vector<std::string>::const_iterator	str_iter;
	for (str_iter = attributes.server_name.begin(); str_iter != attributes.server_name.end(); ++str_iter){
		std::cout << *str_iter << " ";
	}
	std::cout << std::endl;
	std::cout << "-----Location-----" << std::endl;
	std::cout << attributes.location << std::endl;
	std::cout << "-----Max body length-----" << std::endl;
	std::cout << attributes.max_body_length << std::endl;
	std::cout << "-----Redirect-----" << std::endl;
	std::cout << attributes.redirect << std::endl;
	std::cout << "-----Root-----" << std::endl;
	std::cout << attributes.root << std::endl;
	std::cout << "-----Directory listing-----" << std::endl;
	std::cout << attributes.directory_listing << std::endl;
}

bool	Route::checkRequest(const t_http_message &req) const {
	std::string	http_method = req.request_line.method;
	t_methods	request_method;
	if (strcmp(http_method, "GET") == 0)
    {
        request_method = GET;
    }
    else if (strcmp(http_method, "POST") == 0)
    {
        request_method = POST;
    }
    else if (strcmp(http_method, "DELETE") == 0)
    {
        request_method = DELETE;
    }
    else
    {
        // Invalid HTTP request method
        return 1; // throw an exception
    }
	if (attributes.allowed_methods & request_method){
		return 1;
	}
	return 0;
}

const Ressource	*Route::createRessource(const t_http_message &req) const{

} 