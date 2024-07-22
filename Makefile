# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: asfletch <asfletch@student.42heilbronn.    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/03/22 10:21:25 by asfletch          #+#    #+#              #
#    Updated: 2024/07/22 14:57:51 by asfletch         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

.SILENT:

COLOUR_DEFAULT = \033[0m
COLOUR_GREEN = \033[0;32m
COLOUR_BLUE = \033[0;34m
COLOUR_MAGENTA = \033[0;35m
COLOUR_END = \033[0m

CC = c++
RM = rm -f
CFLAGS = -Wall -Wextra -Werror -I./includes/ -g -std=c++98 -fsanitize=address -fsanitize=undefined
SRCS = main.cpp HttpServer.cpp
OBJ_DIR = obj
SRC_DIR = src/
OBJ = $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))
DEPS = $(addprefix $(SRC_DIR)/, $(SRCS)) ./includes/HttpServer.hpp

NAME = webserv

$(NAME): $(OBJ_DIR) $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
	echo "$(COLOUR_GREEN)$(NAME) compiled successfully!$(COLOUR_END)"

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)%.cpp $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

all: $(NAME)

clean:
	$(RM) -rf $(OBJ_DIR)
	@echo "$(COLOUR_BLUE)$(NAME) objects removed successfully..$(COLOUR_END)"

fclean: clean
	$(RM) $(NAME)
	echo "$(COLOUR_MAGENTA)$(NAME) cleaned successfully..$(COLOUR_END)"

re: fclean all

.PHONY: all clean fclean re
