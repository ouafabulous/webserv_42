#include <Ressource.hpp>

RedirectRessource::RedirectRessource(Connexion *conn, std::string const &url) : Ressource(conn)
{
	Logger::info << conn->client_ip_addr << " - Redirect to " << url << std::endl;

	t_headers custom_headers;

	custom_headers["Location"] = url;

	std::string body = "<html>\n";
    body += "<head>\n";
    body += "<title>301 Moved Permanently</title>\n";
    body += "</head>\n";
    body += "<body>\n";
    body += "<h1>Moved Permanently</h1>\n";
    body += "<p>The document has moved <a href=\"" + url + "\">here</a>.</p>\n";
    body += "</body>\n";
    body += "</html>\n";

	conn->pushResponse(http_header_formatter(301, body.length(), "text/html", custom_headers));
	conn->pushResponse(body);
	conn->setRespEnd();
}

RedirectRessource::~RedirectRessource() {}
