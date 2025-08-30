CC = gcc
CFLAGS = -Wall -Wextra -O2
LIBS = -lSDL2 -lSDL2_mixer

TARGET = cl
SRC = cl.c

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

install: all
	cp $(TARGET) /usr/bin/
	
uninstall:
	rm -f /usr/bin/$(TARGET)

clean:
	rm -f $(TARGET)
