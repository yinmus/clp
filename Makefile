CC = gcc

CFLAGS = -Wall -Wextra -O2

LIBS = -lSDL2 -lSDL2_mixer

TARGET = cl

SRC = cl.c

INSTALL_DIR = /usr/local/bin

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

install: $(TARGET)
	mkdir -p $(INSTALL_DIR)
	sudo cp $(TARGET) $(INSTALL_DIR)/
	@echo "Установка завершена. Исполняемый файл находится в $(INSTALL_DIR)"
uninstall:
	rm -f $(INSTALL_DIR)/$(TARGET)
	@echo "Удаление завершено."

clean:
	rm -f $(TARGET)
	@echo "Очистка завершена."

distclean: clean

help:
	@echo "Доступные команды:"
	@echo "  make all       - Собрать программу (по умолчанию)"
	@echo "  make install   - Установить программу в $(INSTALL_DIR)"
	@echo "  make uninstall - Удалить программу"
	@echo "  make clean     - Удалить скомпилированный файл"
	@echo "  make help      - Показать эту справку"

.PHONY: all install uninstall clean distclean help
