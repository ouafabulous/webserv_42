#pragma once
#ifndef TYPE_HPP
#define TYPE_HPP

#include <netinet/in.h>
#include <string>
#include <vector>
#include <map>
#include <Errors.hpp>
// #include <cstdint>
#include <algorithm>
#include <climits>
#include <DirectiveValue.hpp>
// IO

class DirectiveValue; 

typedef unsigned int uint;

typedef int t_fd;

struct t_request_line
{
	std::string method;
	std::string path;
	std::string http_version;
};

struct t_http_message
{
	t_request_line									request_line;
	std::map<std::string, std::string>				header_fields;
	std::vector<char>								body;
};

typedef std::pair<in_addr_t, in_port_t>				t_network_address;

// ROUTER && ROUTE

typedef enum
{
	GET =		0b001,
	POST = 		0b010,
	DELETE = 	0b100
} t_methods;



typedef struct s_attributes
{
	t_methods 										allowed_methods;	// could be GET POST DELETE
	// std::vector<std::string>								server_names;		// defined by header field "Host"
	std::string										server_name;  //just for testing
	std::string										location;			// path requested in the request line
	size_t											max_body_length;
	Errors											error_files;		// path to default error pages -- not implemented in the V0 of the parser
	std::string										redirect;			// uri for redirection,
	std::string										root;				// path to root directory for serving static files
	std::map<std::string, std::string>				cgi_path;			// path to CGI locations -- not implemented int the V0 of the parser
	bool											directory_listing;	// autoindex on/off
	uint											port;
} t_attributes;

typedef enum s_s_tok
{
	TOK_WORD,
	TOK_SP,
	TOK_RL,
	TOK_SC,
	TOK_BR_OP,
	TOK_BR_CL,
	TOK_S_MAX,
} t_s_tok;

typedef enum s_b_tok
{
	TOK_BLOCK,
	TOK_DIRECTIVE,
	TOK_B_MAX,
} t_b_tok;

typedef enum e_chr_class
{
	CHR_SP,					// SPACES
	CHR_WORD,				// WORLD
	CHR_RL,					// ?
	CHR_SC,					// SEMICOLONS
	CHR_BR_OP,
	CHR_BR_CL,					// BRACKETS
	CHR_MAX,
} t_chr_class;

typedef std::pair<t_s_tok, std::string>		t_token;


typedef	enum	e_block_type
{
	BL_SERVER,
	BL_LOCATION,
	BL_MAX,
}	t_block_type;

// const size_t	size_max_array = SSIZE_MAX/sizeof(char);

// union directiveValue {
//     int 							intValue;
//     char[size_max_array]			stringValue;
// };
	// union directiveValueUnion {
	// 	int		_intValue;
	//     char	_stringValue[1023];
	// 	// std::string	stringValue;
	// };

typedef	std::pair<std::string, DirectiveValue> directive;

typedef std::vector<t_token> TokenList;

#define CRLF   "\r\n"

#endif
