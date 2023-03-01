#include <Ressource.hpp>

RedirectRessource::RedirectRessource(Connexion *conn, std::string const &url) : Ressource(conn)
{
	// append header
	conn->pushResponse("HTTP/1.1 301 Moved Permanently\r\n");
	conn->pushResponse("Location: " + url + "\r\n");
	conn->pushResponse("Content-Type: text/html\r\n");
	conn->pushResponse("\r\n");

	// append body
	conn->pushResponse("<html>\n");
	conn->pushResponse("<head>\n");
	conn->pushResponse("<title>301 Moved Permanently</title>\n");
	conn->pushResponse("</head>\n");
	conn->pushResponse("<body>\n");
	conn->pushResponse("<h1>Moved Permanently</h1>\n");
	conn->pushResponse("<p>The document has moved <a href=\"" + url + "\">here</a>.</p>\n");
	conn->pushResponse("</body>\n");
	conn->pushResponse("</html>\n");
}

RedirectRessource::~RedirectRessource() {}
