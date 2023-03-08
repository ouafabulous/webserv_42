#ifndef TYPE_HPP
#define TYPE_HPP

#include <netinet/in.h>
#include <string>
#include <vector>
#include <map>
#include <Errors.hpp>
#include <algorithm>
#include <climits>
#include <queue>
#include <Config.hpp>

typedef unsigned int uint;

typedef int t_fd;

typedef std::map<std::string, std::string> MimeMap;

typedef enum
{
	NO_METHOD,
	GET = 0b001,
	POST = 0b010,
	DELETE = 0b100,
} t_methods;

typedef enum
{
	POLL_CTL_ADD,
	POLL_CTL_MOD,
	POLL_CTL_DEL
} t_poll_action;

struct t_request_line
{
	t_methods method;
	std::string methodVerbose;
	std::string path;
	std::string http_version;
};

typedef std::queue<std::string> t_body;

struct t_http_message
{
	t_request_line request_line;
	std::map<std::string, std::string> header_fields;
	size_t content_length;
	mutable t_body body;
};

typedef std::pair<in_addr_t, in_port_t> t_network_address;

// ROUTER && ROUTE

typedef struct s_attributes
{
	uint port;
	std::vector<std::string> server_name; // defined by the hesder field "Host:"
	std::string location;				  // path requested in the request line
	t_methods allowed_methods;			  // could be GET POST DELETE
	size_t max_body_length;
	std::string redirect;						 // uri for redirection,
	std::string root;							 // path to root directory for serving static files
	std::string index;
	bool directory_listing;						 // autoindex on/off
	std::map<std::string, std::string> cgi_path; // path to CGI locations -- not implemented int the V0 of the parser
	Errors error_files;							 // path to default error pages -- not implemented in the V0 of the parser
	std::string	uploadsFolder;					// path to the uploads folder

	s_attributes() :
		allowed_methods(DEFAULT_ALLOWEDMETHODS),
		max_body_length(DEFAULT_MAXBODYSIZE),
		index(DEFAULT_INDEX),
		directory_listing(DEFAULT_AUTOINDEX) {};
} t_attributes;

typedef enum s_s_tok
{
	TOK_WORD,  // WORD
	TOK_SP,	   // SPACE
	TOK_RL,	   // RETURN LINE
	TOK_SC,	   // SEMI COLON
	TOK_BR_OP, // BRACKET OPEN
	TOK_BR_CL, // BRACKET CLOSE
	TOK_COM,   // COMMENT
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
	CHR_SP,	   // SPACES
	CHR_WORD,  // WORLD
	CHR_RL,	   // RETURN LINE
	CHR_SC,	   // SEMICOLONS OPEN
	CHR_BR_OP, // SEMI COLON CLOSE
	CHR_BR_CL, // BRACKETS
	CHR_COM,   // COMMENT
	CHR_MAX,
} t_chr_class;

typedef std::pair<t_s_tok, std::string> t_token;

typedef enum e_block_type
{
	BL_SERVER,
	BL_LOCATION,
	BL_MAX,
} t_block_type;

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

typedef std::vector<t_token> TokenList;

#define CRLF "\r\n"
#define OK 0
#define NOK 1

class Route;

typedef struct s_cgiInfo
{
	std::string _filePath;
	std::string _queryString;
	std::string _executable;
	s_cgiInfo(const std::string &filePath = "", const std::string &queryString = "", const std::string &executable = "")
		: _filePath(filePath), _queryString(queryString), _executable(executable){};
} t_cgiInfo;

#endif
