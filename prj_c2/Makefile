EXEC = c2_server

SRC = src/server.c
OBJ = $(SRC:.c=.o)

CC = gcc
CFLAGS = -g -Wall -Wextra -pthread

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)
