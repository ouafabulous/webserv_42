#pragma once
#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <IO.hpp>
// #include <Ressource.hpp>
#include <Router.hpp>
#include <Errors.hpp>
#include <Utils.hpp>
#include <Config.hpp>
#include <Server.hpp>
#include <sys/epoll.h>

class Server;

class ListenSocket : public IO
{
public:
	ListenSocket(const t_network_address netAddr);
	~ListenSocket();

	virtual void			read();
	virtual void			write();
	virtual void			closed();
	virtual t_fd			fdDelete();

private:
	const t_fd				l_socket;
	const t_network_address	netAddr;
};

class Connexion : public IO
{
public:
	Connexion(const t_network_address netAddr, const t_fd socket, const Router &router);
	~Connexion();
	virtual void			read();
	virtual void			write();
	virtual void			closed();
	virtual t_fd			fdDelete();
	// t_http_message		get_header();

private:
	void					readHeader();	// called by read() until header is fully received
	void					parseHeader();	// called by read_header() when header is fully received
	bool					readBody();	// called by read() if not read_header()

	const t_fd				c_socket;
	const t_network_address	netAddr;
	const ListenSocket		*listen_socket;
	const Router			&router;

	t_http_message			request;		// struct where header is stored when parsed
	std::string				request_header;	// string where raw header is stored
	bool					header_end;		// bool to define if we finished to read the header
	const Route				*route;			// const or not
	Ressource				*ressource;
};

#endif
