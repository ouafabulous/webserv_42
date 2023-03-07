#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <IO.hpp>
#include <unistd.h>
#include <Errors.hpp>
#include <Utils.hpp>
#include <Config.hpp>
#include <Dechunker.hpp>
#include <queue>

class Server;
class Router;
class Ressource;

class ListenSocket : public IO
{
public:
	ListenSocket(const t_network_address netAddr, const Router &router);
	virtual ~ListenSocket();

	virtual IOEvent read();
	virtual IOEvent closed();
	t_fd getFd() const;

private:
	const t_fd l_socket;
	const t_network_address netAddr;
	struct sockaddr_in address;
	int addr_len;
	const Router &router;
};

class Connexion : public IO
{
public:
	Connexion(const t_network_address netAddr, const t_fd socket, const Router &router, const std::string client_ip_address);
	virtual ~Connexion();
	virtual IOEvent read();
	virtual IOEvent write();
	virtual IOEvent closed();
	t_http_message const &getRequest() const;
	IOEvent setError(std::string log, uint http_error);
	void setRessource(Ressource *_ressource);
	void pushResponse(std::string message);
	void pushResponse(const char *message, size_t n);
	void setRespEnd();
	bool getRespEnd() const;
	bool getBodyParsed() const;
	const std::string client_ip_addr;
	Route const *getRouteCgi() const { return route; };

private:
	IOEvent readHeader(); // called by read() until header is fully received
	IOEvent readBody();	  // called by read() if not read_header()
	// Parsing
	IOEvent parseHeader(std::vector<std::string> &); // called by read_header() when header is fully received
	bool parseRequestLine(std::string &request_line);
	IOEvent parseChunkedBody();
	IOEvent executeRoute();
	bool decodePercent(std::string &uri);

	const t_fd c_socket;
	const t_network_address netAddr;
	const ListenSocket *listen_socket;
	const Router &router;

	// DATA RELATED TO REQUEST PARSING
	t_http_message request;		 // struct where header is stored when parsed
	std::string raw_request;	 // string where raw request is stored
	bool is_header_parsed;		 // bool to define if we finished to read the header
	bool is_request_line_parsed; // first line of request has been received
	bool is_body_parsed;		 // body is fully received
	size_t header_read_size;	 // amount of bytes readed from the connexion for the header part
	size_t body_read_size;	 // amount of bytes readed fron the connexion for the body part
	Dechunker dechunker;		 // object to dechunk http chunk request to a normal body string

	// DATA RELATED TO THE INTERNAL PART AND RESPONSE PART
	const Route *route; // const or not
	Ressource *ressource;
	std::queue<std::string> response;
	bool resp_start; // true if connexion has started to respond to client
	bool resp_end;	 // true if all the response is in the response queue
};

#endif
