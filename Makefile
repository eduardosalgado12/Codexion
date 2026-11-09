# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: edsalgad <edsalgad@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/11/03 11:11:17 by edsalgad          #+#    #+#              #
#    Updated: 2026/11/09 15:23:24 by edsalgad         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	= codexion

CC		= cc
CFLAGS	= -Wall -Wextra -Werror -pthread -I include

SRCS	= src/main.c src/init.c src/utils.c
OBJS	= $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c include/codexion.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re