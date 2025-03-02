#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>

#define BAR_WIDTH 20

void clear_screen() {
    system("clear");
}

void print_ui(int elapsed, int total, int volume, int is_paused) {
    int progress = (elapsed * BAR_WIDTH) / total;
    printf("\rT : %02d:%02d / %02d:%02d  [", elapsed / 60, elapsed % 60, total / 60, total % 60);
    for (int i = 0; i < BAR_WIDTH; i++) {
        printf(i < progress ? "=" : "-");
    }
    printf("]  ");
    printf("V : %d%%  ", volume);
    printf("M : %s   ", is_paused ? "[ ⏸  ]" : "[ ▶ ]");
    fflush(stdout);
}

int get_key(int timeout) {
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

void help_view() {
    char *filename = "help";
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
        return;
    }

    clear_screen();
    printf("---(Press 'q' to exit)---\n");

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file)) {
        printf("%s", buffer);
    }

    fclose(file);

    printf("\n-----------------------\n");

    char input[10];
    while (1) {
        printf(":");
        if (fgets(input, sizeof(input), stdin)) {
            if (strcmp(input, "q\n") == 0) {
                break;
            } else {
                printf("Unknown command. Type 'q' to exit.\n");
            }
        }
    }

    clear_screen();
}

int main(int argc, char *argv[]) {
    if (argc != 3 || strcmp(argv[1], "-c") != 0) {
        fprintf(stderr, "Usage: %s -c <filename>\n", argv[0]);
        return 1;
    }

    char *filename = argv[2];
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

    while (running && Mix_PlayingMusic()) {
        int elapsed = Mix_GetMusicPosition(music);
        print_ui(elapsed, total_time, (volume * 100) / MIX_MAX_VOLUME, is_paused);

        key = get_key(1);
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
            case 'a':
                if (!is_paused) {
                    Mix_PauseMusic();
                    is_paused = 1;
                }
                help_view();
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
