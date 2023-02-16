#include <Dechunker.hpp>
#include <sstream>
#include <Logger.hpp>

Dechunker::Dechunker(std::string &body)
	: body(body),
	  line_state(CHUNKSIZE),
	  chunk_size(0) {}
Dechunker::~Dechunker() {}

IOEvent Dechunker::operator()(std::string &raw_request)
{
	size_t pos = 0;
	std::stringstream size_converter;
	while (1)
	{
		if (line_state == CHUNKSIZE)
		{
			if ((pos = raw_request.find(CRLF)) == std::string::npos)
				break;
			size_converter.clear();
			size_converter << std::hex << raw_request.substr(0, pos);
			if (!(size_converter >> chunk_size))
				return IOEvent(FAIL, NULL, "Chunk size not valid", 413);
			if (chunk_size == 0)
				return IOEvent(SUCCESS);
			line_state = PAYLOAD;
			pos += 2;
		}
		else if (line_state == PAYLOAD)
		{
			if (raw_request.length() < chunk_size)
				break;
			body.append(raw_request.substr(0, chunk_size));
			line_state = NL;
			pos = chunk_size;
		}
		else if (line_state == NL)
		{
			if (raw_request.size() < 2)
				break;
			pos = 2;
			line_state = CHUNKSIZE;
		}
		raw_request.erase(0, pos);
	}
	return IOEvent();
}
