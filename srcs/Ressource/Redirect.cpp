#include <Ressource.hpp>

RedirectRessource::RedirectRessource(Connexion *conn, std::string url) : conn(conn),
																		fd_read(-1),
																		fd_write(-1)
{
}

RedirectRessource::~RedirectRessource() {}
void	RedirectRessource::read() {}
void	RedirectRessource::write() {}
void	RedirectRessource::closed() {}
