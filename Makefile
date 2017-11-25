NAME = Matt_daemon

SRCS_DIR = ./src/

INCLUDE_DIR = ./include

SRC_FILE = matt_daemon.cpp

SRC = $(addprefix $(SRCS_DIR), $(SRC_FILE))

OBJ = $(SRC:%.cpp=%.o)

CC = g++

LFLAGS =

IFLAGS = -I$(INCLUDE_DIR)

CFLAGS = -Wall -Werror -Wextra $(IFLAGS)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) -o $(NAME) $(OBJ) $(LFLAGS)

%.o: %.cpp
	$(CC) -o $@ -c $^ $(CFLAGS)

clean:
	@rm -f $(OBJ)

fclean: clean
	@rm -f $(NAME)

re: fclean all

debug: CFLAGS += -g -D _DEBUG
debug: re

.PHONY: clean fclean re
