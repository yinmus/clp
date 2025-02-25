CC = gcc
CFLAGS = -Wall -Wextra -O2
LIBS = -lSDL2 -lSDL2_mixer

TARGET = audioplayer
SRC = main.c

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

install: all
	mkdir -p ~/.local/bin
	cp $(TARGET) ~/.local/bin/

uninstall:
	rm -f ~/.local/bin/$(TARGET)

clean:
	rm -f $(TARGET)
