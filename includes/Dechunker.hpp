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
	t_body&			body;
	t_chunk_state	line_state;
	size_t			chunk_size;
	size_t&			body_size;
public:
	Dechunker(t_body& body, size_t& body_size);
	~Dechunker();
	IOEvent	operator()(std::string& raw_request);

};

#endif
