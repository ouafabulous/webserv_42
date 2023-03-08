PROGRAM = webserv
HDRS = includes
CXX = g++

TMPDIR = .tmp

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g

ROUTER = $(addprefix Router/, Lexer.cpp Router.cpp Route.cpp Parser.cpp Block.cpp Directive.cpp)
SOCKET = $(addprefix Socket/, ListenSocket.cpp Connexion.cpp Dechunker.cpp IOEvent.cpp)
RESSOURCES = $(addprefix Ressource/, Ressource.cpp CGI.cpp Directory.cpp File.cpp Redirect.cpp)
UTILS = $(addprefix Utils/, Utils.cpp Logger.cpp)
SRCS = $(addprefix srcs/, main.cpp Server.cpp Errors.cpp $(ROUTER) $(SOCKET) $(UTILS) $(RESSOURCES))
OBJS = $(addprefix $(TMPDIR)/, $(SRCS:%.cpp=%.o))
# DOCS_DIR = webapp/docs_html

CXX = c++
RM = rm -rf

all: $(PROGRAM)

$(PROGRAM): $(OBJS) | Makefile # $(DOCS_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ -g

$(OBJS): $(TMPDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -I$(HDRS) -MMD -MF $(TMPDIR)/$*.d $< -o $@

# $(DOCS_DIR):
# 	@docs/doxygen docs/Doxyfile

clean:
	$(RM) $(TMPDIR)

fclean: clean
	$(RM) $(PROGRAM)

re: fclean all

.PHONY: all clean fclean re

-include $(OBJS:%.o=%.d)
