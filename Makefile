.SILENT:

COLOUR_DEFAULT = \033[0m
COLOUR_GREEN = \033[0;32m
COLOUR_BLUE = \033[0;34m
COLOUR_MAGENTA = \033[0;35m
COLOUR_END = \033[0m

CC = c++
RM = rm -f
BASE_CFLAGS = -Wall -Wextra -Werror -std=c++17 -I./includes/
DEBUG_CFLAGS = $(BASE_CFLAGS) -fsanitize=address -fsanitize=undefined ASAN_OPTIONS=detect_leaks=1
RELEASE_CFLAGS = $(BASE_CFLAGS)

LDFLAGS = -lsqlite3

SRCS =	main.cpp \
		server/HttpServer.cpp \
		server/readMethods.cpp \
		server/sendResponse.cpp \
		server/Server.cpp \
		server/register.cpp \
		server/connectionHandler.cpp \
		server/parseRequest/parseRequest.cpp \
		server/parseRequest/parseUtils.cpp \
		server/handleRequest/handleRequest.cpp \
		server/handleRequest/handleGet.cpp \
		server/handleRequest/handlePost.cpp \
		server/handleRequest/handleDelete.cpp \
		server/handleRequest/fillProfile.cpp \
		server/handleRequest/handleCgi.cpp \
		server/serverUtils/errorMethods.cpp \
		server/serverUtils/utils.cpp \
		config/config.cpp \
		config/utils.cpp \
		logs/logs.cpp \
		database/database.cpp \
		database/addUser.cpp \
		database/queryUser.cpp \
		database/deleteProfile.cpp

OBJ_DIR = obj
SRC_DIR = src/
OBJ = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
DEPS = $(SRCS:%.cpp=$(SRC_DIR)/%.cpp) ./includes/HttpServer.hpp ./includes/config.hpp \
	./includes/includes.hpp ./includes/log.hpp ./includes/server.hpp ./includes/structs.hpp \
	./includes/database.hpp

NAME = webserv

# Default target
all: $(NAME)

# Link objects into the executable
$(NAME): CFLAGS = $(RELEASE_CFLAGS)
$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME) $(LDFLAGS)
	echo "$(COLOUR_GREEN)$(NAME) compiled successfully!$(COLOUR_END)"

# Debug build target
debug: CFLAGS = $(DEBUG_CFLAGS)
debug: $(NAME)

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
