#include <Ressource.hpp>

RedirectRessource::RedirectRessource(Connexion *conn, std::string const &url) : Ressource(conn)
{
	std::string body = "<html>\n";
    body += "<head>\n";
    body += "<title>301 Moved Permanently</title>\n";
    body += "</head>\n";
    body += "<body>\n";
    body += "<h1>Moved Permanently</h1>\n";
    body += "<p>The document has moved <a href=\"" + url + "\">here</a>.</p>\n";
    body += "</body>\n";
    body += "</html>\n";

	conn->pushResponse(http_header_formatter(301, body.length(), "text/html"));
	conn->pushResponse(body);
	conn->setRespEnd();
}

RedirectRessource::~RedirectRessource() {}
