EXEC = my_db

SRC = src/main.c src/repl.c src/db.c
OBJ = $(SRC:.c=.o)

CC = gcc
CFLAGS = -g -O2 -Wall -Wextra

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)
