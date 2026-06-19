CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

NAME = webserv

SRC_DIRS = config http server cgi session logger
SRCS = src/main.cpp $(foreach dir,$(SRC_DIRS),$(wildcard src/$(dir)/*.cpp))
OBJS = $(patsubst src/%.cpp,obj/%.o,$(SRCS))
INCLUDES = -Iincludes

obj/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -rf obj/

fclean: clean
	rm $(NAME)

re: fclean all

run: all
	clear
	./$(NAME)

debug: CXXFLAGS += -g -O0
debug: all

debug_re: CXXFLAGS += -g -O0
debug_re: re

.PHONY: all clean fclean re debug debug_re run rrun