#include "Utils.hpp"

int	set_nonblocking(int fd)
{
	if (fd <0)
		return false;
	int	flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		return false;
	return (fcntl(fd, F_SETFL, flags | O_NONBLOCK));
}
