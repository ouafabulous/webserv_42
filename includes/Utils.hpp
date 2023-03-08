#ifndef UTILS_HPP
#define UTILS_HPP

#include <Type.hpp>
#include <IO.hpp>
#include <Logger.hpp>

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <cstring>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <dirent.h>


bool			set_nonblocking(int fd);
bool			poll_util(t_poll_action action, t_fd fd, IO* io_ptr, int flags);
std::string		http_header_formatter(uint status_code, size_t content_length, std::string content_type);
std::string		escape_string(const std::string &str);
size_t          matchingLocation(const std::string& a, const std::string& b);
size_t          matchingChar(const std::string& a, const std::string& b);
t_methods	    methodToEnum(std::string const &method);

std::ostream&	operator<<(std::ostream &out, const t_network_address &netAddr);
int             stringToInt(std::string str);
bool            endsWithSlash(const std::string& str);
bool            fileExists(const char* fileName);
bool            directoryExists(const char* path);
bool            containsSubstring(const std::string& str, const std::string& substr);
std::string     extractBeforeChar(const std::string& inputString, char delimiter);
bool            checkPermissions(const std::string& file_path, const mode_t& mode);
std::string     extractAfterChar(const std::string& inputString, char delimiter);
const MimeMap   &get_mime_map();
const std::string			get_mime(const std::string &file_path);
std::string	    queueToStr(std::queue<std::string> q);  
void            createFolder(const std::string& folderName); 

#endif
