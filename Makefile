.SILENT:

COLOUR_DEFAULT = \033[0m
COLOUR_GREEN = \033[0;32m
COLOUR_BLUE = \033[0;34m
COLOUR_MAGENTA = \033[0;35m
COLOUR_END = \033[0m

CC = c++
RM = rm -f
CFLAGS = -Wall -Wextra -Werror -I./includes/ -g -std=c++98 -fsanitize=address -fsanitize=undefined

SRCS = main.cpp server/HttpServer.cpp server/handleRequest.cpp server/readMethods.cpp \
	server/utils.cpp config/config.cpp
OBJ_DIR = obj
SRC_DIR = src/
OBJ = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
DEPS = $(SRCS:%.cpp=$(SRC_DIR)/%.cpp) ./includes/HttpServer.hpp ./includes/config.hpp

NAME = webserv

# Default target
all: $(NAME)

# Link objects into the executable
$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
	echo "$(COLOUR_GREEN)$(NAME) compiled successfully!$(COLOUR_END)"

$(OBJ_DIR)/%.o: $(SRC_DIR)%.cpp $(DEPS)
	@mkdir -p $(@D)  # Ensure the directory exists
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -rf $(OBJ_DIR)
	@echo "$(COLOUR_BLUE)Objects removed successfully.$(COLOUR_END)"

fclean: clean
	$(RM) $(NAME)
	echo "$(COLOUR_MAGENTA)Executable cleaned.$(COLOUR_END)"

re: fclean all

.PHONY: all clean fclean re
