#include <Ressource.hpp>

CGI::CGI(Connexion *conn, std::string cgi_path) :	conn(conn),
													fd_read(execve(cgi_path.c_str(), NULL, NULL)),
													fd_write()
{
}

CGI::~CGI() {}
void	CGI::read() {}
void	CGI::write() {}
void	CGI::closed() {}
