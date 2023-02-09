#include <Ressource.hpp>

RedirectRessource::RedirectRessource(Connexion *conn, std::string url) : Ressource(conn)
{
}

RedirectRessource::~RedirectRessource() {}
IOEvent	RedirectRessource::closed() {}
