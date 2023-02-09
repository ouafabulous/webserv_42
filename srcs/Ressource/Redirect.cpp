#include <Ressource.hpp>

RedirectRessource::RedirectRessource(Connexion *conn, std::string url) : conn(conn),
																		fd_read(-1),
																		fd_write(-1),
																		reponse(),
																		is_EOF(false)
{
}

RedirectRessource::~RedirectRessource() {}
IOEvent	RedirectRessource::read() {}
IOEvent	RedirectRessource::write() {}
IOEvent	RedirectRessource::closed() {}
