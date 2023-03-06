Please find the project's documentation [here](https://forest-jewel-9bb.notion.site/WebServ-6b7838c8783e4bcd8c97b948bfb0cd2f).

***

# Usage:
./webserv **_configuration_file_**

# Configuration files format:
The format of our configuration files are directly inspired from NGINX.

There is two types of data:
- directives
- block

## Directives
Directives are mades as follow:
```nginx
keyword:	value;
```

## Blocks
Blocks are used to provide contextual data within a scope. The scope is surrounded by curly brackets. There are two kinds of blocks:

- Server block
- Location block

### Server block
Location blocks are used to differentiate the behavior of your server based on the address and port requested by the client.

```NGINX
server {
	listen			8080;				# listening port [REQUIRED]
	server_names	42.fr www.42.fr;	# Will match HTTP Header-field `Host`
	location / { ... }					# zero or more location blocks
}
```

### Location block
Location blocks are used to differentiate the behavior of your server based on the path requested by the client (URI path in the HTTP start-line). They inherit the directives from the parent block (server or location). Each of these scopes defines a route.

## List of available directives
### specific server block directive
- `listen`: *unsigned int* - represents the port on which the virtual server is listening
- `server_names`: *one or more URI separated by space(s)* - matches with the HTTP Header-field **Host**

### one definition directive
- `allowed_methods`: *zero or more flags (`GET` | `POST` | `DELETE`)* separated by space(s) - HTTP request methods allowed for the route
- `client_max_body_size`: *unsigned int* - maximum request body size (in bytes) allowed for the route
- `redirect`: *URI* - request made on this route will be answered by a Moved Permanently (308) response pointing to this URI
- `root`: *path* - path which the server will use to serve files requested by the client
- `index`: *filename* - filename used to resolve a request pointing to a directory path
- `autoindex`:	*(`on` | `off`)* - if *`on`*, a request pointing to a directory path will be answered by an HTML representation of the files contained in the directory
### multiple definition directive
- `cgi_setupt`:	*file-extention* and *path-to-the-binary*, separated by a space - if a request is pointing to a file that ends with the *file-extention*, it will be answerd by the STDOUT of this file given as an argument to the *path-to-the-binary* executable
- `error_file`:	*status-code* and *path-to-error-file*, separated by a space - if an error occured with the same statuts-code as the one set, the server will respond by sending the file at *path-to-error-file*

***

## Examples
### Example 1: Simple server configuration
This configuration sets up a server that listens on port 8080 and responds to requests on the `/` route.

```nginx
server {
	listen 8080;
	server_names 42.fr www.42.fr;
	root /var/www/html;
	index index.html;
	autoindex on;
	location / {
		allowed_methods GET POST;
		client_max_body_size 1024;
	}
}
```

### Example 2: Redirect configuration

This configuration redirects requests to the root route to a new URL.

```nginx
server {
	listen 8080;
	server_names 42.fr
	location / {
		redirect /new-location;
	}
	location /new-location {
		root /var/www/new-location;
		index index.html;
	}
}
```


### Example 3: CGI configuration

This configuration sets up a CGI script to handle requests for `.py` files.

```nginx
server {
	listen 8080;
	server_names 42.fr;
	location / {
		root /var/www/html;
		index index.html;
		cgi_setup .py /usr/bin/python3;
	}
}
```

Note that these are just examples, and you will need to adjust them to match the specific needs of your application.
