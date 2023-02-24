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


IOevent	Route::checkRequest(const t_http_message &req) const {
	t_methods	request_method = req.request_line.method;
	if (attributes.allowed_methods & request_method){
		// je sais pas trop comment utiliser le debugge ici
		return setError("", 400);
		// return 1;
	}
	return IOEvent(); // what does that do?
}



t_cgi	isCGI(std::string const &path){
	if (containsSubstring(path, ".py")){
		return(PYTHON);
	}
	else if (containsSubstring(path, ".php")) {
		return(PHP);
	}
	else if (containsSubstring(path, ".pl")) {
		return(PERL);
	}
	return (NONE);
}

const Ressource	*Route::createRessource(const t_http_message &req, Connexion *conn) const{
	// Where to checkif a path is valid?
	t_request_line	reqLine = req.request_line;
	std::string		completePath = attributes.root + reqLine.path;
	if (reqLine.method & GET){
		t_cgi cgi = isCGI(completePath);
		if (cgi != NONE){
			if (fileExists(extractBeforeChar(completePath, "?")) && check_permissions(completePath, S_IXUSR | S_IXGRP)){
				return (&(CGI(conn, completePath, pathCgi[CGI]))) //pathCGI to define
						}
			else {
				//not found
			}
		}
		else if (fileExists(completePath)) {
			return(&(GetStaticFile(conn, completePath)));
		}
		else if(directoryExists(completePath)){
			if (attributes.redirect){
				return(&(RedirectRessource(conn, attributes.directory_listing)));
			}
			if(attributes.directory_listing){
				return(&(GetDirectory(conn, completePath)));	
			}
			else if (attributes.index.size() > 0){
				for (std::vector<std::string>::iterator indexIt = attributes.index.begin; indexIt != attributes.index.end(); indexIt++){
					if (fileExists(*indexIt)){
						return(&(GetStaticFile(conn, *indexIt)));
					} 
				}
				if (fileExists(reqLine.path+"index.html")){
					return(&(GetStaticFile(conn, completePath+"index.html")));
				}
				return () // I need to return a message error: 403 not found
			}
		}
	}
	else if (reqLine.method & POST) {
		return(&(PostStaticFile(conn, completePath)));
	}
	else if (reqline.method & DELETE) {
		if (fileExists(completePath)){
			return (&(DeleteStaticFile()))
		}
		else {
			// not found error
		}
	}
} 

void			handle(t_http_message &req,Connexion *connexion) const{
	if (req.request_line.method & GET) {

	}
}