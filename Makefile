PROGRAM = webserv
HDRS = includes

TMPDIR = .tmp

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g

ROUTER = $(addprefix Router/, Lexer.cpp Router.cpp Route.cpp)
SOCKET = $(addprefix Socket/, ListenSocket.cpp Connexion.cpp)
SRCS = $(addprefix srcs/, main.cpp Server.cpp Utils.cpp $(ROUTER) $(SOCKET))
OBJS = $(addprefix $(TMPDIR)/, $(SRCS:.cpp=.o))

CXX = c++
RM = rm -rf

all: $(PROGRAM)

$(PROGRAM): $(OBJS) | Makefile
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJS): $(TMPDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -I$(HDRS) -MMD -MF $(TMPDIR)/$*.d $< -o $@

clean:
	$(RM) $(TMPDIR)

fclean: clean
	$(RM) $(PROGRAM)

re: fclean all

.PHONY: all clean fclean re

-include $(OBJS:.o=.d)
