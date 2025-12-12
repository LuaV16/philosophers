NAME = philo

SRC = main.c utils.c parse.c init.c

OBJ := $(SRC:%.c=%.o)

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread -O3

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re : fclean all