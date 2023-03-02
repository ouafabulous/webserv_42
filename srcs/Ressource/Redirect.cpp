#include <Ressource.hpp>

RedirectRessource::RedirectRessource(Connexion *conn, std::string const &url) : Ressource(conn)
{
	// append header
	std::string body = "<html>\n";
	body += "HTTP/1.1 301 Moved Permanently\r\n";
	body += "Location: " + url + "\r\n";
	body += "Content-Type: text/html\r\n";
	body += "\r\n";

	// append body
	body += "<html>\n";
	body += "<head>\n";
	body += "<title>301 Moved Permanently</title>\n";
	body += "</head>\n";
	body += "<body>\n";
	body += "<h1>Moved Permanently</h1>\n";
	body += "<p>The document has moved <a href=\"" + url + "\">here</a>.</p>\n";
	body += "</body>\n";
	body += "</html>\n";

	conn->pushResponse(body);
	conn->setRespEnd();
}

RedirectRessource::~RedirectRessource() {}
