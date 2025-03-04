#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>

#define BAR_WIDTH 20

void ext_enc(const char *filename, char *output, size_t size);

void cls() {
    system("clear");
}

void UI(int elapsed, int total, int volume, int is_paused, int show_filename, const char *filename) {
    int progress = (elapsed * BAR_WIDTH) / total;
    int volume_bar = (volume * 5) / MIX_MAX_VOLUME;

    cls();

    if (show_filename) {
        char name_without_extension[256];
        ext_enc(filename, name_without_extension, sizeof(name_without_extension));
        printf("%s\n", name_without_extension);
    }

    printf("\rT : %02d:%02d / %02d:%02d  [", elapsed / 60, elapsed % 60, total / 60, total % 60);
    for (int i = 0; i < BAR_WIDTH; i++) {
        printf(i < progress ? "=" : "-");
    }
    printf("]  V : [");
    for (int i = 0; i < 5; i++) {
        printf(i < volume_bar ? "#" : "-");
    }
    printf("] %d%%  ", (volume * 100) / MIX_MAX_VOLUME);
    printf("M : %s   ", is_paused ? "Pause" : "Play");
    fflush(stdout);
}

int keys(int timeout) {
    struct termios oldt, newt;
    fd_set set;
    struct timeval tv;
    int ch = -1;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);
    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    if (select(STDIN_FILENO + 1, &set, NULL, NULL, &tv) > 0) {
        ch = getchar();
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

void help() {
    char path[256];
    snprintf(path, sizeof(path), "%s/Документы/.help/help.1", getenv("HOME"));
    char command[512];
    snprintf(command, sizeof(command), "man %s", path);
    system(command);
    cls();
}

void ext_enc(const char *filename, char *output, size_t size) {
    const char *last_slash = strrchr(filename, '/');
    const char *last_dot = strrchr(filename, '.');

    if (last_slash == NULL) last_slash = filename;
    else last_slash++;

    if (last_dot == NULL || last_dot < last_slash) {
        snprintf(output, size, "%s", last_slash);
    } else {
        size_t length = last_dot - last_slash;
        snprintf(output, size, "%.*s", (int)length, last_slash);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    cls();

    char *filename = argv[1];
    Mix_Music *music = NULL;

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "Mixer initialization failed: %s\n", Mix_GetError());
        goto cleanup_sdl;
    }

    music = Mix_LoadMUS(filename);
    if (!music) {
        fprintf(stderr, "Failed to load music: %s\n", Mix_GetError());
        goto cleanup_mixer;
    }

    if (Mix_PlayMusic(music, 1) == -1) {
        fprintf(stderr, "Failed to play music: %s\n", Mix_GetError());
        goto cleanup_music;
    }

    int volume = MIX_MAX_VOLUME / 2;
    Mix_VolumeMusic(volume);

    int running = 1, is_paused = 0;
    int total_time = Mix_MusicDuration(music);
    int key;
    int seek_step = 5;
    int show_filename = 0;

    while (running && Mix_PlayingMusic()) {
        int elapsed = Mix_GetMusicPosition(music);
        UI(elapsed, total_time, volume, is_paused, show_filename, filename);

        key = keys(1);
        switch (key) {
            case '=': case '+':
                if (volume < MIX_MAX_VOLUME) volume += MIX_MAX_VOLUME / 20;
                Mix_VolumeMusic(volume);
                break;
            case '-':
                if (volume > 0) volume -= MIX_MAX_VOLUME / 20;
                Mix_VolumeMusic(volume);
                break;
            case 'm':
                volume = MIX_MAX_VOLUME;
                Mix_VolumeMusic(volume);
                break;
            case 'n':
                volume = 0;
                Mix_VolumeMusic(volume);
                break;
            case ' ':
                if (is_paused) Mix_ResumeMusic();
                else Mix_PauseMusic();
                is_paused = !is_paused;
                break;
            case 'q':
                Mix_HaltMusic();
                running = 0;
                break;
            case 'h':
                if (Mix_SetMusicPosition(Mix_GetMusicPosition(music) - seek_step) == -1) {
                    fprintf(stderr, "Seek not supported for this format\n");
                }
                break;
            case 'l':
                if (Mix_SetMusicPosition(Mix_GetMusicPosition(music) + seek_step) == -1) {
                    fprintf(stderr, "Seek not supported for this format\n");
                }
                break;
            case 'H':
                if (Mix_SetMusicPosition(Mix_GetMusicPosition(music) - 10) == -1) {
                    fprintf(stderr, "Seek not supported for this format\n");
                }
                break;
            case 'L':
                if (Mix_SetMusicPosition(Mix_GetMusicPosition(music) + 10) == -1) {
                    fprintf(stderr, "Seek not supported for this format\n");
                }
                break;
            case 'N':
                show_filename = !show_filename;
                break;
            case 'a':
                if (!is_paused) {
                    Mix_PauseMusic();
                    is_paused = 1;
                }
                help();
                if (is_paused) {
                    Mix_ResumeMusic();
                    is_paused = 0;
                }
                break;
        }
    }

    printf("\n");

cleanup_music:
    if (music) Mix_FreeMusic(music);
cleanup_mixer:
    Mix_CloseAudio();
cleanup_sdl:
    SDL_Quit();
    return 0;
}
