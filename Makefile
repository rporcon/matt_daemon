SRCS_DIR = ./src/

INCLUDE_DIR = ./include


SRC_FILE_MATT = svrclt_common.cpp matt_daemon.cpp logger.cpp server.cpp \
				rc4_cipher.cpp

SRC_MATT = $(addprefix $(SRCS_DIR), $(SRC_FILE_MATT))

OBJ_MATT = $(SRC_MATT:%.cpp=%.o)


SRC_FILE_BEN = ben_afk.cpp svrclt_common.cpp keygen.cpp rc4_cipher.cpp \
			   logger.cpp

SRC_BEN = $(addprefix $(SRCS_DIR), $(SRC_FILE_BEN))

OBJ_BEN = $(SRC_BEN:%.cpp=%.o)


CC = g++

LFLAGS = -ltar

IFLAGS = -I$(INCLUDE_DIR)

CFLAGS = -Wall -Werror -Wextra -std=c++14 $(IFLAGS)

all: Matt_daemon Ben_AFK

Matt_daemon: $(OBJ_MATT)
	$(CC) -o $@ $^ $(LFLAGS)

Ben_AFK: $(OBJ_BEN)
	$(CC) -o $@ $^ $(LFLAGS)

%.o: %.cpp
	$(CC) -o $@ -c $^ $(CFLAGS)

clean:
	@rm -f $(OBJ_MATT)
	@rm -f $(OBJ_BEN)

fclean: clean
	@rm -f Matt_daemon
	@rm -f Ben_AFK

re: fclean all

debug: CFLAGS += -g -D _DEBUG
debug: re

.PHONY: clean fclean re
