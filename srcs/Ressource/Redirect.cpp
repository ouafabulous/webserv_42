#include <Ressource.hpp>

RedirectRessource::RedirectRessource(Connexion *conn, std::string url) : Ressource(conn)
{
	response = "HTTP/1.1 301 Moved Permanently\r\n";
	response += "Location: " + url + "\r\n";
	response += "Content-Type: text/html\r\n";
	response += "\r\n";
	response += "<html>\n";
	response += "<head>\n";
	response += "<title>301 Moved Permanently</title>\n";
	response += "</head>\n";
	response += "<body>\n";
	response += "<h1>Moved Permanently</h1>\n";
	response += "<p>The document has moved <a href=\"" + url + "\">here</a>.</p>\n";
	response += "</body>\n";
	response += "</html>\n";

}

RedirectRessource::~RedirectRessource() {}
IOEvent	RedirectRessource::write()
{
	//if (send(whichFD?, response.c_str(), response.size(), 0) == -1)
	//	return IOEvent(FAIL, this, "RedirectRessource::write() failed");
}
