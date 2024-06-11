##
## EPITECH PROJECT, 2023
## RayCasting
## File description:
## Makefile
##

NAME = Wolf3D

SRC = src/main.c \
	  src/game/analyze_event.c \
	  src/game/display.c \
	  src/pause/analyze_event.c \
	  src/pause/display.c \

OBJ = $(SRC:.c=.o)

CFLAGS = -I./include -lcsfml-graphics -lcsfml-window -lcsfml-system -lm

all: $(NAME)

$(NAME): $(OBJ)
	gcc -o $(NAME) $(OBJ) $(CFLAGS)

run: 
	@make -s $(NAME)
	@./$(NAME)
	@make -s fclean

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all