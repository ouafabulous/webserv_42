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
	std::cout << "Listen: \t\t" << attributes.port << std::endl;

	std::cout << "Server Names: \t\t";
	std::vector<std::string>::const_iterator	str_iter;
	for (str_iter = attributes.server_name.begin(); str_iter != attributes.server_name.end(); ++str_iter){
		std::cout << *str_iter << " ";
	}
	std::cout << std::endl;

	std::cout << "Location: \t\t" << attributes.location << std::endl;

	std::cout << "Max body length: \t" << attributes.max_body_length << std::endl;

	std::cout << "Root: \t\t\t" << attributes.root << std::endl;

	std::cout << "Methods allowed: \t";
	if (attributes.allowed_methods & GET) {
		std::cout << "GET ";
	}
	if (attributes.allowed_methods & POST) {
		std::cout << "POST ";
	}
	if (attributes.allowed_methods & DELETE){
		std::cout << "DELETE";
	}
	std::cout << std::endl;

	std::cout << "Directory listing: \t" << attributes.directory_listing << std::endl;

	std::cout << "CGI setup: \t\t";
	for (std::map<std::string, std::string>::const_iterator it = attributes.cgi_path.begin(); it != attributes.cgi_path.end(); ++it){
		std::cout << it->first << "->" << it->second << " ";
	}
	std::cout << std::endl;

	std::cout << "Redirect: \t" << attributes.redirect << std::endl;
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



int	Route::isCGI(std::string const &path){
	std::map<std::string, std::string>::iterator	cgiIt;
	int	i = 0;
	for (cgiIt = cgiMap.begin(); cgiIt != cgiMap.end(); cgiIt++) {
		if (containsSubstring(path, it->first)) {
			return (i);
		}
		i++;
	}
	return (-1);
}

IOEvent	Route::setRessource(const t_http_message &req, Connexion *conn) const{
	// Where to checkif a path is valid?
	t_request_line	reqLine = req.request_line;
	std::string		completePath = attributes.root + reqLine.path;
	if (attributes.redirect){
		conn->ressource = new RedirectRessource(conn, attributes.redirect+reqLine.path);
	}
	bool cgi = isCGI(completePath);
	if (cgi > -1){
		if (fileExists(extractBeforeChar(completePath, "?"))){
			if (check_permissions(completePath, S_IXUSR | S_IXGRP)){
				t_cgi_info	cgiInfo( extractBeforeChar(completePath, "?"),extractAfterChar(completePath, "?"), cgiMap[cgi]->second);
				return (new CGI(conn, cgiInfo));
			}
			else {
				// throw permission denied
			}
		} 
	else if (reqLine.method & GET){
			if (fileExists(completePath)) {
				if (checkPermissions(completePath, S_IRUSR | S_IRGRP)) {
					return(&(GetStaticFile(conn, completePath)));
				}
				else {
					//throw permissions exception
				}
			}
			else if(directoryExists(completePath)){
				if (checkPermissions(completePath, S_IRUSR | S_IRGRP)) {
					if (fileExists(completePath +"index.html")) {
						
					}
				}
				else {
					//
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
	epoll_util(EPOLL_CTL_DEL, fd_read, this, EPOLLIN);
	if (close(fd_read) == -1)
	{
		conn->setError("Error closing the file", 500);
		throw std::runtime_error("GetStaticFile::~GetStaticFile() Close failed");
	}
}

IOEvent	GetStaticFile::read()
{
	size_t ret = ::read(fd_read, buffer, BUFFER_SIZE);

	if (ret == -1)
	{
		close(fd_read);
		return conn->setError("Error reading the file", 500);
	}
	if (!ret)
		return closed();
	if (ret == 0)
		is_EOF = true;
	conn->append_response(buffer, ret);
	return IOEvent();
}

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