#include <Ressource.hpp>

RedirectRessource::RedirectRessource(Connexion *conn, const std::string url) : Ressource(conn)
{
	// append header
	conn->append_response("HTTP/1.1 301 Moved Permanently\r\n", 0);
	conn->append_response("Location: " + url + "\r\n", 0);
	conn->append_response("Content-Type: text/html\r\n", 0);
	conn->append_response("\r\n", 0);

	// append body
	conn->append_response("<html>\n", 0);
	conn->append_response("<head>\n", 0);
	conn->append_response("<title>301 Moved Permanently</title>\n", 0);
	conn->append_response("</head>\n", 0);
	conn->append_response("<body>\n", 0);
	conn->append_response("<h1>Moved Permanently</h1>\n", 0);
	conn->append_response("<p>The document has moved <a href=\"" + url + "\">here</a>.</p>\n", 0);
	conn->append_response("</body>\n", 0);
	conn->append_response("</html>\n", 0);
}

RedirectRessource::~RedirectRessource() {}
