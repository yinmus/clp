# clp

## Легкий, консольный аудио плеер

### Форматы:
- `MP3`
- `WAV`
- `OGG`

### Зависимости:

- `SDL2` (libsdl2-dev) — библиотека для работы с мультимедиа.
- `SDL2_mixer` (libsdl2-mixer-dev) — расширение для воспроизведения аудиофайлов.

___

## Запуск и установка
**Установка:**

[`installer`](installer.sh) / [`Makefile`](Makefile)


**Запуск**

```bash
cl -c /path/to/file.ext
```

```bash
cl -c file.ext
```
___


`plus` (+) - Увеличить громкость 

`minus` (-) - Уменьшить громкость 

`l` (l) - Промотать 5 секунд   /    `L` (shift + l) - Промотать на 10 секунд

`h` (h) - Отмотать 5 секунд   /    `H` (shift + h) - Отмотать на 10 секунд

`r` (r) - sLoop = !sLoop

`N` (N) - sOutputName = !sOutputName 

`q` (q) - Выйти


___

