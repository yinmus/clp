#!/bin/sh

APP_NAME="cl"
SRC_FILE="cl.c"
INSTALL_DIR="/usr/local/bin"

echo "Установка $APP_NAME..."
echo "Выберите ваш дистрибутив:"
echo "1. Arch/Manjaro"
echo "2. Ubuntu/Debian"
read -r DISTRO

echo "Установить зависимости? [y/n]:"
read -r INSTALL_DEPS

if [ "$INSTALL_DEPS" = "y" ]; then
    echo "Установка зависимостей..."
    if [ "$DISTRO" = "1" ]; then
        sudo pacman -S --noconfirm base-devel sdl2 sdl2_mixer
    elif [ "$DISTRO" = "2" ]; then
        sudo apt update && sudo apt install -y build-essential libsdl2-dev libsdl2-mixer-dev
    fi
fi

echo "Компилировать? [y/n]:"
read -r COMPILE

if [ "$COMPILE" = "y" ]; then
    gcc -o $APP_NAME $SRC_FILE $(pkg-config --cflags --libs sdl2 SDL2_mixer)
    sudo mv $APP_NAME $INSTALL_DIR/
    sudo chmod +x $INSTALL_DIR/$APP_NAME
fi

echo "Запустить? [y/n]:"
read -r RUN

if [ "$RUN" = "y" ]; then
    cl -c /path/to/mp3file.mp3
fi

echo "Установка завершена."
