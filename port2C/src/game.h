#ifndef GAME_H
#define GAME_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#define SPRITE_COUNT 48
#define BLOCK_COUNT 17

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
    SDL_Texture* sprites[SPRITE_COUNT];
    SDL_Texture* blocks[BLOCK_COUNT];
    Mix_Chunk* sounds[6];
    IceObject objs[10];
    int gameMode;
    int lastKey;
    int running;
} GameState;

#endif