#pragma once
#ifndef TYPE_HPP
#define TYPE_HPP

#include <map>
#include <netinet/in.h>
#include <string>
#include <vector>

// IO

typedef int t_fd;

struct t_request_line
{
	std::string method;
	std::string path;
	std::string http_version;
};

struct t_http_message
{
	t_request_line request_line;
	std::map<std::string, std::string> header_fields;
	std::vector<char> body;
};

struct t_network_address
{
	in_addr_t address;
	in_port_t port;
};

// ROUTER && ROUTE

typedef enum
{
	GET = 0b001,
	POST = 0b010,
	DELETE = 0b100
} t_methods;

typedef struct s_attributes
{
	t_methods methods_allowed;			   // could be GET POST DELETE
	std::vector<std::string> server_names; // defined by header field "Host"
	std::string location;				   // path requested in the request line
	size_t max_body_length;
	std::map<uint, std::string> error_files;	 // path to default error pages
	std::string redirect;						 // uri for redirection,
	std::string root;							 // path to root directory for serving static files
	std::map<std::string, std::string> cgi_path; // path to CGI locations
	bool directory_listing;						 // autoindex on/off
} t_attributes;

typedef enum s_l_tok
{
	TOK_WORD,
	TOK_SP,
	TOK_RL,
	TOK_SC,
	TOK_BR,
} t_t_tok;

typedef enum s_b_tok
{
	TOK_BLOCK,
	TOK_DIRECTIVE,
} t_b_tok;

typedef enum e_chr_class
{
	CHR_SP,
	CHR_WORD,
	CHR_RL,
	CHR_SC,
	CHR_BR,
} t_chr_class;

static const t_chr_class g_get_chr_class[255] =
	{
		[' '] = CHR_SP,
		['\t'] = CHR_SP,
		[';'] = CHR_SC,
		['}'] = CHR_BR,
		['!' ... ':'] = CHR_WORD,
		[';'] = CHR_SC,
		['<' ... 'z'] = CHR_WORD,
		['{'] = CHR_BR,
		['|'] = CHR_WORD,
		['}'] = CHR_BR,
		['~'] = CHR_WORD,
};

static const t_t_tok		g_get_tok_type[5] = {
	[CHR_SP] = TOK_SP,
	[CHR_WORD] = TOK_WORD,
	[CHR_RL] = TOK_RL,
	[CHR_SC] = TOK_SC,
	[CHR_BR] = TOK_BR,
};



#endif
