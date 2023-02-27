#ifndef DECHUNKER_HPP
# define DECHUNKER_HPP

#include <Type.hpp>
#include <IO.hpp>

typedef enum {
	CHUNKSIZE,
	PAYLOAD,
	NL
}	t_chunk_state;

class Dechunker
{
private:
	std::string&	body;
	t_chunk_state	line_state;
	size_t			chunk_size;
public:
	Dechunker(std::string& body);
	~Dechunker();
	IOEvent	operator()(std::string& raw_request);

};

#endif
