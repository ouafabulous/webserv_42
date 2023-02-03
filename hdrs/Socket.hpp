#pragma once
#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "Server.hpp"
#include "Ressource.hpp"

class ListenSocket : public IO
{
public:
	ListenSocket(const t_network_address netAddr);
	~ListenSocket();

	virtual void			read();
	virtual void			write();
	virtual void			closed();
	virtual t_fd			fd_delete();

private:
	const t_fd				socket;
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
	virtual t_fd			fd_delete();
	// t_http_message		get_header();	

private:
	void					read_header();	// called by read() until header is fully received
	void					parse_header();	// called by read_header() when header is fully received
	bool					read_body();	// called by read() if not read_header()

	const t_fd				socket;
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
