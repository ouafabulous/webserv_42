#pragma once
#ifndef CONFIG_HPP
# define CONFIG_HPP

#define CONN_TO_LISTEN	64
#define BUFFER_SIZE		65000
#define MAX_HEADER_SIZE	4096
#define CH_PERM			0644
#define DIRECTIVE_NAME_LENGTH 5000

#define HTTP_VERSION	"HTTP/1.1"

// CONFIG FILE FIELDS
#define LISTEN "listen"
#define SERVERNAMES "server_names"
#define ALLOWEDMETHODS "allowed_methods"
#define MAXBODYSIZE "client_max_body_size"
#define REDIRECT "redirect"
#define ROOT "root"
#define INDEX "index"
#define AUTOINDEX "autoindex"
#define CGISETUP "cgi_setup"
#define ERRORFILE "error_file"
#define UPLOADS "uploads"

#define DEFAULT_ALLOWEDMETHODS static_cast<t_methods>(GET | POST | DELETE)
#define DEFAULT_MAXBODYSIZE	1000
#define DEFAULT_INDEX "index.html"
#define DEFAULT_AUTOINDEX true
#define DEFAULT_UPLOADS "uploads/"

#endif
