#include <Ressource.hpp>

RedirectRessource::RedirectRessource(Connexion *conn, std::string const &url) : Ressource(conn)
{
	// append header
	conn->append_response("HTTP/1.1 301 Moved Permanently\r\n");
	conn->append_response("Location: " + url + "\r\n");
	conn->append_response("Content-Type: text/html\r\n");
	conn->append_response("\r\n");

	// append body
	conn->append_response("<html>\n");
	conn->append_response("<head>\n");
	conn->append_response("<title>301 Moved Permanently</title>\n");
	conn->append_response("</head>\n");
	conn->append_response("<body>\n");
	conn->append_response("<h1>Moved Permanently</h1>\n");
	conn->append_response("<p>The document has moved <a href=\"" + url + "\">here</a>.</p>\n");
	conn->append_response("</body>\n");
	conn->append_response("</html>\n");
}

RedirectRessource::~RedirectRessource() {}
