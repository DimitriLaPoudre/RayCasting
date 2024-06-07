##
## EPITECH PROJECT, 2023
## RayCasting
## File description:
## Makefile
##

NAME = Wolf3D

SRC = src/main.c

OBJ = $(SRC:.c=.o)

CFLAGS = -lcsfml-graphics -lcsfml-window -lm

all: $(NAME)

$(NAME): $(OBJ)
	gcc -o $(NAME) $(OBJ) $(CFLAGS)

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all