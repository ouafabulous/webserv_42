#include <Ressource.hpp>

ErrorRessource::ErrorRessource(Connexion *conn, std::string error_response) : Ressource(conn)
{
	//append header
	//conn->append_response("HTTP/1.1 404 Not Found\r\n", 0);
	//conn->append_response("Content-Type: text/html\r\n", 0);
	//conn->append_response("\r\n", 0);
}

ErrorRessource::~ErrorRessource() {}
