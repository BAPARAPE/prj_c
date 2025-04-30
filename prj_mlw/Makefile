TARGET = libmlw.so

SRC = src/hooks.c src/knock.c src/malware.c

OBJ = $(SRC:.c=.o)

CC = gcc
CFLAGS = -fPIC -O2 -Wall -shared -ldl -pthread

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
