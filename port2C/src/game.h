#ifndef GAME_H
#define GAME_h
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

typedef struct {
    int x, y;
    int look;
    int type;
    int dir;
    int step;
    int pos;
} IceObject;

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture** sprites;
    SDL_Texture** blocks;
    Mix_Chunk* sounds[6];
    IceObject objs[10];
    int gameMode;
    int lastKey;
    bool running;
} GameState;

#endif