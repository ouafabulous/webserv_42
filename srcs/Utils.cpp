#include <Utils.hpp>
#include <Server.hpp>

void	set_nonblocking(int fd)
{
	if (fd <0)
		return ;
	int	flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		throw std::runtime_error("set_nonblocking function failed");
}


void	epoll_util(int action, t_fd fd, IO* io_ptr, int flags) {
	epoll_event	event;
	event.data.ptr = io_ptr;
	event.events = flags;
	if (epoll_ctl(Server::epollfd, action, fd, &event) == -1)
		throw std::runtime_error("epoll_ctl failed");
}

// STREAM OVERLOAD

std::ostream&	operator<<(std::ostream &out, const t_network_address &netAddr)
{
	const static std::string	text[5] = {"TOK_WORD", "TOK_SP\t", "TOK_RL\t", "TOK_SC\t", "TOK_BR\t"};

	if (netAddr.first) {
		unsigned char *p = (unsigned char *)&netAddr.first;
		out << (int)p[0] << "." << (int)p[1] << "." << (int)p[2] << "." << (int)p[3] << ":";
	}
	out << ntohs(netAddr.second);
    return out;
}
