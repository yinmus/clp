1CC := gcc
CFLAGS := -Wall -Wextra -O2
LIBS := -lSDL2 -lSDL2_mixer

TARGET := cl
SRC := cl.c

INSTALL_DIR := /usr/local/bin
H_INSTALL_DIR := $(HOME)/Документы/.help

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

install: $(TARGET)
	@echo "Установка..."
	@mkdir -p $(INSTALL_DIR) $(H_INSTALL_DIR)
	@sudo cp $(TARGET) $(INSTALL_DIR)/
	@cp help.1 $(H_INSTALL_DIR)/
	@echo "✅ Установлено в $(INSTALL_DIR)"

uninstall:
	@echo "Удаление..."
	@sudo rm -f $(INSTALL_DIR)/$(TARGET)
	@echo "❌ Удалено из $(INSTALL_DIR)"

clean:
	@echo "Очистка..."
	@rm -f $(TARGET)
	@echo "✅ Файлы удалены."


help:
	@echo "Доступные команды:"
	@echo "  make            - Собрать программу (по умолчанию)"
	@echo "  make install    - Установить в $(INSTALL_DIR)"
	@echo "  make uninstall  - Удалить программу"
	@echo "  make clean      - Удалить скомпилированный файл"
	@echo "  make help       - Показать эту справку"

.PHONY: all install uninstall clean distclean help
