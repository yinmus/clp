#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "config.h"
#define BAR_WIDTH 30

bool running;

void handle_signal(int signal) { running = 0; }

void cls() { system(scls); }

// Interface Output
void print_ui(int Elapsed, int Total, int Volume, int Ispaused, bool OutputName,
              char *NameFile, bool islooping) {
  int progress = (Elapsed * BAR_WIDTH) / Total;

  // Progress Bar
  printf("\rT : %02d:%02d / %02d:%02d  [", Elapsed / 60, Elapsed % 60,
         Total / 60, Total % 60);
  for (int i = 0; i < BAR_WIDTH; i++) {
    printf(i < progress ? "=" : "-");
  }
  printf("]  ");
  printf("V : %d%%  ", Volume);
  printf("M : %s   ", Ispaused ? "pause" : "play");
  printf("L : %s   ", islooping ? "loop" : "once");

  if (OutputName) {
    printf("N: %s", NameFile);
  } else {
    printf("N: %*s", (int)strlen(NameFile), " ");
  }

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

int main(int argc, char *argv[]) {
  if (argc != 3 || strcmp(argv[1], "-p") != 0) {
    fprintf(stderr, "Usage: %s -p <filename>\n", argv[0]);
    return 1;
  }
  signal(SIGINT, handle_signal);

  cls();

  char *filename = argv[2];

  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    fprintf(stderr, "Mixer initialization failed: %s\n", Mix_GetError());
    SDL_Quit();
    return 1;
  }

  Mix_Music *music = Mix_LoadMUS(filename);
  if (!music) {
    fprintf(stderr, "Failed to load music: %s\n", Mix_GetError());
    Mix_CloseAudio();
    SDL_Quit();
    return 1;
  }

  float Hvolume = (MIX_MAX_VOLUME * sdefaultsound) / 100;
  int volume = (int)Hvolume;
  Mix_VolumeMusic(volume);

  int running = 1, is_paused = 0;
  int total_time = Mix_MusicDuration(music); // in seconds
  int key;
  int sshortstep = 5;

  if (Mix_PlayMusic(music, isloop ? -1 : 1) == -1) {
    fprintf(stderr, "Failed to play music: %s\n", Mix_GetError());
    Mix_FreeMusic(music);
    Mix_CloseAudio();
    SDL_Quit();
    return 1;
  }

  printf("\n");

  while (running && (Mix_PlayingMusic() || is_paused)) {
    int elapsed = Mix_GetMusicPosition(music); // in seconds
    print_ui(elapsed, total_time, (volume * 100) / MIX_MAX_VOLUME, is_paused,
             soutputname, filename, isloop);

    key = get_key(1);
    switch (key) {

    case koutname:
      soutputname = !soutputname;
      break;

    case ksound_p:
      if (volume < MIX_MAX_VOLUME) {
        volume += 5;
        if (volume > MIX_MAX_VOLUME) {
          volume = MIX_MAX_VOLUME;
        }
        Mix_VolumeMusic(volume);
      }
      break;

    case ksound_m:
      if (volume >= sshortstep) {
        volume -= sshortstep;
        if (volume < 0) {
          volume = 0;
        }
        Mix_VolumeMusic(volume);
      }
      break;

    case kmaxvol:
      volume = MIX_MAX_VOLUME;
      Mix_VolumeMusic(volume);
      break;

    case kmute:
      volume = 0;
      Mix_VolumeMusic(volume);
      break;

    case kpause:
      if (is_paused) {
        Mix_ResumeMusic();
      } else {
        Mix_PauseMusic();
      }
      is_paused = !is_paused;
      break;

    case ktobeg:

      Mix_HaltMusic();
      if (Mix_PlayMusic(music, isloop ? -1 : 1) == -1) {
        fprintf(stderr, "Failed to restart music: %s\n", Mix_GetError());
      }
      break;

    case kloop:

      if (srewindaftswitch) {
        unsigned int current_pos = Mix_GetMusicPosition(music);

        isloop = !isloop;
        Mix_HaltMusic();
        if (Mix_PlayMusic(music, isloop ? -1 : 1) == -1) {
          fprintf(stderr, "Failed to restart music: %s\n", Mix_GetError());
        }
        Mix_SetMusicPosition(current_pos);

      } else if (!srewindaftswitch) {

        isloop = !isloop;
        Mix_HaltMusic();
        if (Mix_PlayMusic(music, isloop ? -1 : 1) == -1) {
          fprintf(stderr, "Failed to restart music: %s\n", Mix_GetError());
        }
      }
      break;

    case kquit:
      Mix_HaltMusic();
      running = 0;
      break;

    case kshortback:
      if (Mix_GetMusicPosition(music) >= sshortstep) {
        if (Mix_SetMusicPosition(Mix_GetMusicPosition(music) - sshortstep) ==
            -1) {
          fprintf(stderr, "Seek not supported for this format\n");
        }
      } else {
        if (Mix_SetMusicPosition(0) == -1) {
          fprintf(stderr, "Seek not supported for this format\n");
        }
      }
      break;

    case klongback:

      if (Mix_GetMusicPosition(music) >= slongstep) {
        if (Mix_SetMusicPosition(Mix_GetMusicPosition(music) - slongstep) ==
            -1) {
          fprintf(stderr, "Seek not supported for this format\n");
        }
      } else {
        if (Mix_SetMusicPosition(0) == -1) {
          fprintf(stderr, "Seek not supported for this format\n");
        }
      }
      break;

    case kshortforwd:
      if (Mix_SetMusicPosition(Mix_GetMusicPosition(music) + sshortstep) ==
          -1) {
        fprintf(stderr, "Seek not supported for this format\n");
      }
      break;

    case klongforwd:
      if (Mix_SetMusicPosition(Mix_GetMusicPosition(music) + slongstep) == -1) {
        fprintf(stderr, "Seek not supported for this format\n");
      }
      break;
    }

    if (!Mix_PlayingMusic() && !is_paused && !isloop) {
      running = 0;
    }
  }

  printf("\n");

  Mix_HaltMusic();
  Mix_FreeMusic(music);
  Mix_CloseAudio();
  SDL_Quit();

  return 0;
}
